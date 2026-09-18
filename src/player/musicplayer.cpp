#include "musicplayer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>


MusicPlayer::MusicPlayer(QWidget *parent)
    : QWidget{parent}
{
    this->setWindowIcon(QIcon(":/icon/icon.png"));

    if (!DbManager::getInstance().isValid()) {
        QMessageBox::critical(nullptr, "启动失败", "无法加载数据库，应用将退出。");
        return;
    }

    m_mediator = new AppMediator(this); // 创建应用中介者

    m_controller = new PlayerController(this); // 创建播放控制器
    m_songManager = new SongManager(this); // 创建列表管理器

    m_uiMain = new UIMain(this); // 创建主界面
    m_uiSideBar = new UISideBar(this); // 创建侧边栏
    m_uiSearch = new UISearch(this); // 创建搜索栏
    m_detailWidget = new MusicDetailWidget(m_uiMain); // 创建音乐全屏页

    // 为歌部分的界面的QListView设置模型
    m_uiMain->setModel(m_uiMain->listView(), m_songManager->localModel());
    m_uiMain->setModel(m_uiMain->collectListView(), m_songManager->collectModel());
    m_uiMain->setModel(m_uiMain->songListView(), m_songManager->playlistModel());
    m_uiMain->setModel(m_uiMain->remoteListView(), m_songManager->remoteModel());
    m_uiSearch->setModel(m_uiSearch->searchListView(), m_songManager->searchModel());


    // 初始化布局
    initLayout();

    m_mediator->setPlayer(m_controller);
    m_mediator->setListManager(m_songManager);
    m_mediator->setUIMain(m_uiMain);
    m_mediator->setUISideBar(m_uiSideBar);
    m_mediator->setUISearch(m_uiSearch);
    m_mediator->setDetailWidget(m_detailWidget);

    // 连接信号与槽
    m_mediator->connectSignal();

    // 创建系统托盘
    createTrayIcon();

    // 加载配置
    loadSettings();
}


void MusicPlayer::initLayout()
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

    m_detailWidget->hide(); // 隐藏全屏页
    m_detailWidget->stackUnder(m_uiMain->controlBar()); // 将主界面控制栏置于全屏页上面
}


void MusicPlayer::saveSettings()
{
    QSettings s("Luo", "MusicPlayer");

    QModelIndex index = m_songManager->currentIndex(); // 获取当前播放的音乐index
    if(!index.isValid()) return;

    const QSortFilterProxyModel *proxyModel = qobject_cast<const QSortFilterProxyModel*>(index.model()); // 获取当前播放音乐列表的代理模型

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>(); // 获取该代理模型的id

    s.setValue("playlistLastNumber", m_songManager->playlistLastNumber()); // 保存目前处于哪个歌单中
    s.setValue("playPage",  m_songManager->playPage()); // 保存当前播放的歌曲处于哪一页中
    s.setValue("proxyId", QVariant::fromValue(id)); // 保存当前播放音乐所属的代理模型id
    s.setValue("currentRow", index.row()); // 保存当前音乐在播放列表中的行号
    s.setValue("position", m_controller->position()); // 保存当前音乐的播放进度
    s.setValue("volume", m_controller->volume()); // 保存当前的音量大小
    s.setValue("playMode", static_cast<int>(m_songManager->mode())); // 保存当前的播放模式
}


void MusicPlayer::loadSettings()
{
    QSettings s("Luo", "MusicPlayer");

    int playPage = s.value("playPage", Page::Local).toInt(); // 获取上次播放位于哪一页中

    QPushButton* btn = m_uiSideBar->getSideBtnOfPage(playPage); // 根据页面ID获取切换到对应页面的按钮

    // 如果上次播放是歌单页
    if(playPage == Page::PlayList){
        int playlistLastNumber = s.value("playlistLastNumber", 0).toInt(); // 获取上次播放位于哪个歌单中
        btn = m_uiSideBar->findSonglistBtn(playlistLastNumber);
    }

    if(btn) btn->click();

    int row = s.value("currentRow", 0).toInt(); // 获取上次播放音乐在列表中的行号，默认为0
    ProxyId id = s.value("proxyId", QVariant::fromValue(ProxyId::Local)).value<ProxyId>(); // 获取上次播放音乐index所属的代理模型id
    QModelIndex index = m_songManager->proxyModel(id)->index(row, 0); // 根据代理Id获取代理模型，并再根据row获取对应的音乐index

    if(!index.isValid()) return;

    m_songManager->setCurrentIndex(index); // 设置当前播放的音乐在列表中的行号

    m_songManager->setMode(static_cast<PlayMode>(s.value("playMode", 0).toInt())); // 设置当前的播放模式

    m_uiMain->doubleClickPlay(index, false); // 设置当前播放的音乐的标题、作者、封面等信息，并不自动播放

    int playProgress = s.value("position", 0).toInt(); // 获取上次部分的播放进度，默认为0

    // 当音乐加载完成后，设置播放进度，并暂停播放
    connect(m_controller->mediaPlayer(), &QMediaPlayer::mediaStatusChanged, this, [this, playProgress](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::LoadedMedia) {
            m_controller->setPlayProgress(playProgress);
        }
    }, Qt::SingleShotConnection);


    m_controller->setVolume(s.value("volume", 20).toInt()); // 设置当前的音量大小
}


void MusicPlayer::createTrayIcon()
{
    // 如果系统托盘可用
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);  // 指定父对象，自动管理内存
    m_trayIcon->setIcon(QIcon(":/icon/icon.png"));
    m_trayIcon->setToolTip(tr("我的应用"));

    // 创建菜单
    m_trayMenu = new QMenu(this);
    QAction *quitAction = new QAction(tr("退出"), this);
    quitAction->setIcon(QIcon(":/icon/quit.png"));

    connect(quitAction, &QAction::triggered, [this](){
        m_isQuitting = true;
        this->close();
    });

    connect(m_trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        // 判断触发的原因是否为“双击”
        if (reason == QSystemTrayIcon::DoubleClick) {
            // 【双击要执行的代码】：通常是显示/还原主窗口
            if (this->isMinimized()) {
                this->showNormal(); // 如果最小化了，还原
            } else {
                this->show();       // 如果隐藏了，显示
            }
            this->activateWindow(); // 将窗口置顶并获取焦点
        }
    });

    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    m_trayIcon->show();
}


void MusicPlayer::closeEvent(QCloseEvent *event)
{
    if (!m_isQuitting && m_trayIcon && m_trayIcon->isVisible()) {
        hide();  // 隐藏到托盘
        event->ignore();  // 忽略关闭事件
        return;
    }

    saveSettings(); // 关闭程序时保存当前配置

    event->accept();
}

void MusicPlayer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 当窗口大小改变时，调整音乐详情页的大小和位置
    if(m_detailWidget){
        m_detailWidget->setGeometry(0, 0, this->width(), this->height());
    }
}

void MusicPlayer::keyPressEvent(QKeyEvent *event)
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




