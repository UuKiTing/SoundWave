#include "soundwave.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>


SoundWave::SoundWave(QWidget *parent)
    : QWidget{parent}
{

    m_mediator = new AppMediator(this); // 创建应用中介者

    m_controller = new PlayerController(this); // 创建播放控制器
    m_songManager = new SongManager(this); // 创建列表管理器

    m_uiMain = new UIMain(this); // 创建主界面

    m_uiSideBar = new UISideBar(this); // 创建侧边栏
    m_uiSearch = new UISearch(this); // 创建搜索栏
    m_detailWidget = new MusicDetailWidget(m_uiMain); // 创建音乐全屏页

    viewBindModel(m_uiMain, m_uiSearch, m_songManager); // 为视图绑定模型


    // 设置中介者的管理对象
    m_mediator->setPlayer(m_controller);
    m_mediator->setListManager(m_songManager);
    m_mediator->setUIMain(m_uiMain);
    m_mediator->setUISideBar(m_uiSideBar);
    m_mediator->setUISearch(m_uiSearch);
    m_mediator->setDetailWidget(m_detailWidget);


    m_mediator->connectSignal();
}

bool SoundWave::initialize()
{
    this->setWindowIcon(QIcon(":/icon/icon.png"));

    if (!DbManager::getInstance().isValid()) {
        QMessageBox::critical(nullptr, "启动失败", "无法加载数据库，应用将退出。");
        return false;
    }

    initLayout();

    if(!createTrayIcon()){
        qDebug() << "创建系统托盘失败！";
        return false;
    }

    m_trayIcon->show();

    connect(m_songManager, &SongManager::songsLoaded, this, &SoundWave::loadSettings);

    return true;
}


void SoundWave::initLayout()
{
    QHBoxLayout *HBox = new QHBoxLayout(this);
    HBox->setContentsMargins(0, 0, 0, 0);
    HBox->addWidget(m_uiSideBar);
    HBox->setSpacing(0);

    QVBoxLayout *VBox = new QVBoxLayout;
    VBox->setContentsMargins(0, 0, 0, 0);
    VBox->setSpacing(0);
    VBox->addWidget(m_uiSearch);
    VBox->addWidget(m_uiMain);

    HBox->addLayout(VBox);

    m_detailWidget->hide();
    m_detailWidget->stackUnder(m_uiMain->controlBar()); // 将主界面控制栏置于全屏页上面
}


void SoundWave::saveSettings()
{
    QSettings s("Luo", "SoundWave");

    s.setValue("palylistPlayingNumber", m_songManager->playlistPlayingNumber()); // 保存处于播放状态的歌单的歌单序号
    s.setValue("playPage",  m_songManager->playingPage()); // 保存当前播放的歌曲所处的页面类型
    s.setValue("currentRow", m_songManager->currentRow()); // 保存当前歌曲的行号
    s.setValue("position", m_controller->playProgress()); // 保存当前音乐的播放进度
    s.setValue("volume", m_controller->volume()); // 保存当前的音量大小
    s.setValue("playMode", static_cast<int>(m_songManager->playMode())); // 保存当前的播放模式
}


void SoundWave::loadSettings()
{
    if(m_isLoadedSetting) return;

    QSettings s("Luo", "SoundWave");

    m_controller->setVolume(s.value("volume", 20).toInt()); // 设置当前的音量大小

    int playPage = s.value("playPage", MainPage::Local).toInt(); // 获取上次播放歌曲处于的主页面类型
    QPushButton* sideBtn = m_uiSideBar->getSideBtnOfPage(playPage); //  根据页面ID获取侧边栏按钮

    // 如果上次播放位于歌单页面
    if(playPage == MainPage::PlayList){
        int playlisPlayingNumber = s.value("palylistPlayingNumber", 0).toInt(); // 获取上次播放位于的歌单序号
        sideBtn = m_uiSideBar->findSonglistBtn(playlisPlayingNumber); // 根据歌单序号获取对应的歌单按钮
    }

    if(!sideBtn) return;
    sideBtn->click();

    int row = s.value("currentRow", 0).toInt(); // 获取上次播放歌曲的行号
    QModelIndex index = m_uiMain->currentListView()->model()->index(row, 0); // 获取当前页面的视图所对应的模型并获取其行号为row的模型索引

    if(!index.isValid()) return;

    m_songManager->setCurrentIndex(index); // 设置当前播放的音乐在列表中的行号
    m_songManager->setPlayMode(static_cast<PlayMode>(s.value("playMode", 0).toInt())); // 设置当前的播放模式

    m_uiMain->doubleClickPlay(index, false); // 双击播放模型索引是index的歌曲

    int playProgress = s.value("position", 0).toInt(); // 获取上次部分的播放进度

    // 当歌曲音频加载完成后，设置播放进度
    connect(m_controller, &PlayerController::mediaStatusChanged, this, [this, playProgress](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::LoadedMedia) {
            m_controller->setPlayProgress(playProgress);
        }
    }, Qt::SingleShotConnection);

    m_isLoadedSetting = true;
}

bool SoundWave::createTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return false;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icon/icon.png"));
    m_trayIcon->setToolTip(tr("我的应用"));

    // 创建托盘菜单
    m_trayMenu = new QMenu(this);

    QAction *quitAction = new QAction(QIcon(":/icon/quit.png"), tr("退出"), this);

    connect(quitAction, &QAction::triggered, [this](){
        m_isQuitting = true;
        this->close();
    });

    connect(m_trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        // 判断触发的原因是否为“双击”
        if (reason == QSystemTrayIcon::DoubleClick) {
            // 判断是否最小化
            if (this->isMinimized()) {
                this->showNormal();
            } else {
                this->show();
            }
            this->activateWindow(); // 将窗口置顶并获取焦点
        }
    });

    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    return true;
}

void SoundWave::viewBindModel(UIMain *uiMain, UISearch *uiSearch, SongManager *manager)
{
    uiMain->localListView()->setModel(manager->localModel());
    uiMain->collectListView()->setModel(manager->collectModel());
    uiMain->playlistView()->setModel(manager->playlistModel());
    uiMain->remoteListView()->setModel(manager->remoteModel());
    uiSearch->searchListView()->setModel(manager->searchModel());
}


void SoundWave::closeEvent(QCloseEvent *event)
{
    if (!m_isQuitting && m_trayIcon && m_trayIcon->isVisible()) {
        hide();  // 隐藏
        event->ignore();  // 忽略关闭事件
        return;
    }

    saveSettings(); // 关闭程序时保存当前配置

    event->accept();
}

void SoundWave::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 当窗口大小改变时，调整音乐详情页的大小和位置
    if(m_detailWidget){
        m_detailWidget->setGeometry(0, 0, this->width(), this->height());
    }
}

void SoundWave::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space){
        m_controller->playOrPause();
    }
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left){
        emit m_uiMain->songSkipped(false);
    }
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right){
        emit m_uiMain->songSkipped(true);
    }

    QWidget::keyPressEvent(event);
}




