#include "soundwave.h"
#include "config_manager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>

SoundWave::SoundWave(QWidget *parent)
    : QWidget{parent}
{
    m_controller = new PlayerController(this); // 创建播放控制器
    m_songManager = new SongManager(this); // 创建列表管理器
    m_mainWindow = new MainWindow; // 创建主窗口

    if (!DbManager::getInstance().isValid()) {
        QMessageBox::critical(nullptr, "启动失败", "无法加载数据库，应用将退出。");
        return;
    }

    bindViewAndModel(m_mainWindow->uiMain(), m_mainWindow->uiSearch(), m_songManager);

    if(!m_mainWindow->initialize()){
        qDebug() << "MainWindow init failed!";
        return;
    }

     // 创建应用中介者
    m_mediator = new AppMediator(m_controller,
                                 m_songManager,
                                 m_mainWindow,
                                 this);

    // 创建配置管理器
    m_configManager = new ConfigManager(this);

    loadUserState();
    recordUserState();

    connect(m_songManager, &SongManager::songsLoaded, m_configManager, &ConfigManager::loadSetting);

    connect(m_mainWindow, &MainWindow::settingsSaved, m_configManager, &ConfigManager::saveSetting);

    m_initalized = true;
}

SoundWave::~SoundWave()
{
    delete m_mainWindow;
}

void SoundWave::show()
{
    Q_ASSERT(m_mainWindow != nullptr);

    if(!m_initalized) return;

    m_mainWindow->show();
}

void SoundWave::bindViewAndModel(UIMain *uiMain, UISearch *uiSearch, SongManager *manager)
{
    uiMain->localListView()->setModel(manager->localModel());
    uiMain->collectListView()->setModel(manager->collectModel());
    uiMain->playlistView()->setModel(manager->playlistModel());
    uiMain->remoteListView()->setModel(manager->remoteModel());
    uiSearch->searchListView()->setModel(manager->searchModel());
}

bool SoundWave::isInitialized()
{
    return m_initalized;
}

void SoundWave::loadUserState()
{
    connect(m_configManager, &ConfigManager::volumeChanged, m_controller, &PlayerController::setVolume);

    connect(m_configManager, &ConfigManager::playmodeChanged, m_songManager, &SongManager::setPlayMode);

    connect(m_configManager, &ConfigManager::currentRowChanged, this, [this](int row){
        QModelIndex index = m_mainWindow->uiMain()->currentListView()->model()->index(row, 0); // 获取当前页面的视图所对应的模型并获取其行号为row的模型索引
        if(index.isValid()) {
            m_songManager->setCurrentIndex(index); // 设置当前播放的音乐在列表中的行号
            m_mainWindow->uiMain()->doubleClickPlay(index, false); // 双击播放模型索引是index的歌曲
        }
    });

    connect(m_configManager, &ConfigManager::positionChanged, this, [this](int position){
        // 当歌曲音频加载完成后，设置播放进度
        connect(m_controller, &PlayerController::mediaStatusChanged, this, [this, position](QMediaPlayer::MediaStatus status){
            if (status == QMediaPlayer::LoadedMedia) {
                m_controller->setPlayProgress(position);
            }
        }, Qt::SingleShotConnection);
    });

    connect(m_configManager, &ConfigManager::pageChanged, this, [this](MainPage page, int playlist_id){
        QAbstractButton* sideBtn = m_mainWindow->uiSideBar()->getSideBtnOfPage(page); //  根据页面ID获取侧边栏按钮
        if(page == MainPage::PlayList){
            sideBtn = m_mainWindow->uiSideBar()->findPlaylist(playlist_id); // 根据歌单序号获取对应的歌单按钮
        }

        if(!sideBtn) return;

        sideBtn->click();
    });
}

void SoundWave::recordUserState()
{
    connect(m_controller, &PlayerController::volumeChanged, m_configManager, &ConfigManager::setVolume);
    connect(m_controller, &PlayerController::positionChanged, m_configManager, &ConfigManager::setPosition);
    connect(m_songManager, &SongManager::playModeChanged, m_configManager, &ConfigManager::setPlayMode);
    connect(m_songManager, &SongManager::playPageChanged, m_configManager, &ConfigManager::setPlayPage);
    connect(m_songManager, &SongManager::currentRowChanged, m_configManager, &ConfigManager::setCurrentRow);
    connect(m_songManager, &SongManager::currentPlaylistChanged, m_configManager, &ConfigManager::setCurrentPlaylist);
}



