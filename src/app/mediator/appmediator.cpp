#include "appmediator.h"
#include "logging.h"
#include "contextmenu.h"
#include "model_roles.h"
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QTimer>
#include <QListView>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

AppMediator::AppMediator(
    PlayerController *controller,
    SongManager *songManager,
    MainWindow *mainWindow,

    QObject *parent
    )
    : QObject{parent},
    m_controller(controller),
    m_songManager(songManager),
    m_mainWindow(mainWindow)
{
    Q_ASSERT(m_controller != nullptr);
    Q_ASSERT(m_songManager != nullptr);
    Q_ASSERT(m_mainWindow != nullptr);

    connectSignals();
}

void AppMediator::connectSignals()
{
    playConnect();
    progressSliderConnect();
    volumeSliderConnect();
    pageConnect();
    collectConnect();
    searchConnect();
    playlistConnect();
    detailWidgetConnect();
    contextMenuConnect();
}

void AppMediator::playConnect()
{
    // 设置播放按钮Icon
    connect(m_controller, &PlayerController::playbackStateChanged, m_mainWindow->uiMain(), &UIMain::setPlayBtnIcon);

    // 双击播放
    connect(m_mainWindow->uiMain(), &UIMain::songPlayed, this, &AppMediator::playMusic);

    // 手动控制播放/停止
    connect(m_mainWindow->uiMain(), &UIMain::songPlayedOrPaused, m_controller, &PlayerController::playOrPause);
    connect(m_mainWindow->uiSideBar(), &UISideBar::songPlayedOrPaused, m_controller, &PlayerController::playOrPause);

    // 下/上一首
    connect(m_mainWindow->uiMain(), &UIMain::songSkipped, this, &AppMediator::skipMusic);

    // 播放结束自动下一首
    connect(m_controller, &PlayerController::mediaStatusChanged, [this](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::MediaStatus::EndOfMedia) skipMusic(true);
    });

    connect(m_mainWindow->uiMain(), &UIMain::playModeChanged, m_songManager, &SongManager::changePlayMode);

    connect(m_songManager, &SongManager::playModeChanged, m_mainWindow->uiMain(), &UIMain::changePlayMode);

    // 播放错误提示
    connect(m_controller, &PlayerController::playbackError, this, [this](const QString &msg){
        QMessageBox::warning(nullptr, tr("Playback error"), msg);
    });
}

void AppMediator::progressSliderConnect()
{
    // 设置进度条范围
    connect(m_controller, &PlayerController::durationChanged, m_mainWindow->uiMain(), &UIMain::setProgressSliderRange);

    // 当前进度条时长（文本）
    connect(m_controller, &PlayerController::positionChanged, m_mainWindow->uiMain(), &UIMain::setCurDuration);

    // 播放器进度同步给进度条滑块
    connect(m_controller, &PlayerController::positionChanged, m_mainWindow->uiMain(), &UIMain::setProgressValue);

    // 拖动进度条时，同进度值给时长文本
    connect(m_mainWindow->uiMain(), &UIMain::progressChanged, m_mainWindow->uiMain(), &UIMain::setCurDuration);

    // 拖动开始：标记正在拖动
    connect(m_mainWindow->uiMain(), &UIMain::sliderPressed, m_mainWindow->uiMain(), &UIMain::progressDragging);

    // 拖动结束：标记结束拖动
    connect(m_mainWindow->uiMain(), &UIMain::sliderReleased, this, [this](){
        m_mainWindow->uiMain()->progressNodragging();
        m_controller->setPlayProgress(m_mainWindow->uiMain()->progressValue());
    });

    //播放进度同步到歌词
    connect(m_controller, &PlayerController::positionChanged, m_mainWindow->detailWidget(), &MusicDetailWidget::onAudioPositionChanged);
}

void AppMediator::volumeSliderConnect()
{
    // 音量条和播放器音量同步
    connect(m_mainWindow->uiMain()->volumeSlider(), &QSlider::valueChanged, m_controller, &PlayerController::setVolume);
    connect(m_controller, &PlayerController::volumeChanged, m_mainWindow->uiMain(), &UIMain::setVolumeValue);
}

void AppMediator::pageConnect()
{
    connect(m_mainWindow->uiSideBar(), &UISideBar::pageChanged, m_mainWindow->uiMain(), &UIMain::switchStackedWidget);
}

void AppMediator::collectConnect()
{
    connect(m_mainWindow->uiMain(), &UIMain::songCollected, this, &AppMediator::collectSong);

    connect(&ContextMenu::getInstance(), &ContextMenu::songCollected, this, &AppMediator::collectSong);
}

void AppMediator::searchConnect()
{
    connect(m_mainWindow->uiSearch()->searchBar(), &QLineEdit::textChanged, m_songManager->searchModel(), &SearchProxyModel::setKeyWord);
    connect(m_mainWindow->uiSearch(), &UISearch::songPlayRequest, m_mainWindow->uiMain(), &UIMain::doubleClickPlay);
}

void AppMediator::playlistConnect()
{
    UISideBar *sideBar = m_mainWindow->uiSideBar();

    connect(sideBar, &UISideBar::playlistClicked, m_mainWindow->uiMain(), &UIMain::updatePlaylist);

    // 侧边栏歌单点击，更新主界面歌单歌曲列表
    connect(sideBar, &UISideBar::playlistUpdated, m_songManager->playlistModel(), [this](int playlist_id){
        m_songManager->playlistModel()->setAllowedSongIds(m_playlistRepo.queryPlaylistId(playlist_id));
    });

    // 侧边栏右键播放歌单
    connect(sideBar, &UISideBar::playlistPlayed, m_mainWindow->uiMain()->playlistBtn(), &QPushButton::click);

    // 重命名歌单
    connect(sideBar, &UISideBar::playlistNameChanged, this, [this](const QString& name, int playlist_id){
        if(m_playlistRepo.updatePlaylistName(name, playlist_id)){
            ContextMenu::getInstance().updatePlaylistName(name, playlist_id);
            m_mainWindow->uiMain()->setPlaylistName(name);
        }
    });

    connect(sideBar, &UISideBar::playlistDeleted, this, [this](int playlist_id){
        if(m_playlistRepo.deletePlaylist(playlist_id)){
            m_mainWindow->uiSideBar()->deletePlaylist(playlist_id);
            ContextMenu::getInstance().removePlaylist(playlist_id);
        }
    });

    connect(sideBar, &UISideBar::playlistAdded, this, [this](const QString& name){
        return m_playlistRepo.createPlaylist(name);
    });

    m_mainWindow->uiSideBar()->createPlaylists(m_playlistRepo.queryPlaylists());
}

void AppMediator::detailWidgetConnect()
{
    connect(m_mainWindow->uiMain(), &UIMain::detailWidgetShowed,  m_mainWindow->detailWidget(), &MusicDetailWidget::setVisible);
    connect(m_mainWindow->uiMain(), &UIMain::detailWidgetShowed, m_mainWindow->uiSideBar(), &UISideBar::setHidden);
    connect(m_mainWindow->uiMain(), &UIMain::detailWidgetShowed, m_mainWindow->uiSearch(), &UISearch::setHidden);
}

void AppMediator::contextMenuConnect()
{
    auto &contextMenu = ContextMenu::getInstance();

    connect(&contextMenu, &ContextMenu::songRemoved, this, &AppMediator::removeSong);

    connect(&contextMenu, &ContextMenu::songPlayed, this, [this](int row){
        QListView *view = m_mainWindow->uiMain()->currentListView();
        QModelIndex index = view->model()->index(row, 0);

        if(index.isValid()){
            m_mainWindow->uiMain()->doubleClickPlay(index, true);
        }
    });

    connect(&contextMenu, &ContextMenu::downloadProgressChanged, m_songManager, &SongManager::setProgress);
    connect(&contextMenu, &ContextMenu::songAdded, m_songManager, &SongManager::appendSong);


    contextMenu.setPlaylistDataSource(
    [this](int song_id){
        return m_playlistRepo.findPlaylistsBySong(song_id);
    },
    [this](int playlist_id, int song_id){
        return m_playlistRepo.insertSongToPlaylist(playlist_id, song_id);
    },
    [this](int playlist_id){
            return m_playlistRepo.songCountInPlaylist(playlist_id);
    },
    [this](const QString &path, int playlist_id, int cover_id){
        return m_playlistRepo.updatePlaylistCover(path, playlist_id, cover_id);
    });


    contextMenu.addPlaylists(m_playlistRepo.queryPlaylists());
}

void AppMediator::collectSong(bool isCollect, const QModelIndex &index)
{
    QModelIndex idx;

    int removeRow = -1;

    if(!index.isValid()) {
        idx = SongManager::mapToSource(m_songManager->currentIndex());
        removeRow = m_songManager->currentRow();
    }
    else {
        idx = SongManager::mapToSource(index);
        removeRow = index.row();
    }

    if(!idx.isValid()) return;

    if(!m_songManager->setFavorite(idx, isCollect)){
        qCWarning(mediatorLog) << (isCollect ? "收藏失败" : "取消收藏失败");
        return;
    };

    int song_id = idx.data(Roles::Id).toInt();
    if(song_id == m_songManager->currentIndex().data(Roles::Id).toInt()){
        m_mainWindow->uiMain()->collectIconToggle(isCollect);
    }

    if(!isCollect && removeRow >= 0){
        updatePlaybackAfterRemoved(removeRow);
    }
}

void AppMediator::handleLocalSongRemoved(int song_id, int row)
{
    auto *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_mainWindow->uiMain()->currentListView()->model());

    if(!proxyModel){
        return;
    }

    const QModelIndex index = proxyModel->index(row, 0);

    QMessageBox::StandardButton reply = QMessageBox::question(
        nullptr,
        "确认",
        "确定要删除这个歌曲吗?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if(reply == QMessageBox::No || !m_songRepo.removeSong(song_id)){
        return;
    }

    m_songManager->setInvalid(index);
}

void AppMediator::handleCollectSongRemoved(int song_id, int row)
{
    auto *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_mainWindow->uiMain()->currentListView()->model());

    if(!proxyModel){
        return;
    }

    const QModelIndex index = proxyModel->index(row, 0);

    collectSong(false, index);
}

void AppMediator::handlePlaylistSongRemoved(int song_id, int row)
{
    auto *uiSideBar = m_mainWindow->uiSideBar();

    auto *btn = uiSideBar->findPlaylist(uiSideBar->currentSelectPlaylist());

    if(!btn){
        return;
    }

    PlayListInfo info = btn->property("playlist").value<PlayListInfo>();

    if(!m_playlistRepo.removeSongToPlaylist(info.id, song_id)){
        return;
    }

    btn->click();
}

void AppMediator::updatePlaybackAfterRemoved(int removedRow)
{
    auto *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_mainWindow->uiMain()->currentListView()->model());

    m_songManager->setListRows(proxyModel);

    ProxyId proxyId = proxyModel->property("proxyId").value<ProxyId>();

    if(proxyId != m_songManager->currentProxyId() ||
        removedRow != m_songManager->currentRow()){
        return;
    }

    const int rowCount = proxyModel->rowCount();

    if(rowCount == 0){
        return;
    }

    int nextRow = removedRow % rowCount;

    m_mainWindow->uiMain()->doubleClickPlay(proxyModel->index(nextRow, 0)
                            , true);
}

void AppMediator::skipMusic(bool isNext)
{
    QModelIndex nextIndex = m_songManager->setNextIndex(isNext);

    m_mainWindow->uiMain()->setCurrentIndex(nextIndex);
    m_mainWindow->uiMain()->setPlayStyle(nextIndex);

    m_songManager->setPlayingStatus(nextIndex);

    m_controller->skipSong(nextIndex);

    m_mainWindow->detailWidget()->flushDetail(nextIndex);
}

void AppMediator::playMusic(const QModelIndex &index, bool autoPlay)
{
    if(!index.isValid()) return;

    const QSortFilterProxyModel *proxyModel = qobject_cast<const QSortFilterProxyModel*>(index.model());

    if(!proxyModel) return;

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>();

    m_songManager->setProxyId(id);

    MainPage page = m_songManager->pageOfProxyId(id);

    if(page != m_songManager->playingPage()) m_songManager->setPlayPage(page);

    if(id == ProxyId::Playlist)
        m_songManager->setCurrentPlaylist(m_mainWindow->uiSideBar()->currentSelectPlaylist());
    else
        m_songManager->setCurrentPlaylist(-1);

    m_songManager->setCurrentIndex(index);
    m_songManager->setPlayingStatus(index);
    m_songManager->setListRows(proxyModel);

    m_mainWindow->uiMain()->setCurrentIndex(index);

    m_controller->playSong(index, autoPlay); // 交给音乐控制器来真正播放歌曲

    m_mainWindow->detailWidget()->flushDetail(index); // 刷新歌曲详情页的内容
}

void AppMediator::removeSong(int song_id, int row)
{
    switch(m_mainWindow->uiMain()->currentPage()){
    case MainPage::Collect:{
        handleCollectSongRemoved(song_id, row);
        break;
    }
    case MainPage::PlayList:{
        handlePlaylistSongRemoved(song_id, row);
        break;
    }
    case MainPage::Local:{
        handleLocalSongRemoved(song_id, row);
        break;
    }
    default:
        return;
    }

    updatePlaybackAfterRemoved(row);
}
