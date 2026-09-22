#include "appmediator.h"
#include "logging.h"
#include "image_loader_global.h"
#include "contextmenu.h"
#include "model_roles.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QTimer>
#include <QListView>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

AppMediator::AppMediator(QObject *parent)
    : QObject{parent}
{


}

void AppMediator::setPlayer(PlayerController *controller)
{
    m_controller = controller;
}

void AppMediator::setListManager(SongManager *listManager)
{
    m_songManager = listManager;
}

void AppMediator::setUIMain(UIMain *uiMain)
{
    m_uiMain = uiMain;
}

void AppMediator::setUISideBar(UISideBar *uiSideBar)
{
    m_uiSideBar = uiSideBar;
}

void AppMediator::setUISearch(UISearch *uiSearch)
{
    m_uiSearch = uiSearch;
}


void AppMediator::setDetailWidget(MusicDetailWidget *detailWidget)
{
    m_detailWidget = detailWidget;
}


void AppMediator::connectSignal()
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
    connect(m_controller, &PlayerController::playbackStateChanged, m_uiMain, &UIMain::setPlayBtnIcon);

    // 双击播放
    connect(m_uiMain, &UIMain::songPlayed, this, &AppMediator::playMusic);

    // 手动控制播放/停止
    connect(m_uiMain, &UIMain::songPlayedOrPaused, m_controller, &PlayerController::playOrPause);
    connect(m_uiSideBar, &UISideBar::songPlayedOrPaused, m_controller, &PlayerController::playOrPause);

    // 下/上一首
    connect(m_uiMain, &UIMain::songSkipped, this, &AppMediator::skipMusic);

    // 播放结束自动下一首
    connect(m_controller, &PlayerController::mediaStatusChanged, [this](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::MediaStatus::EndOfMedia) skipMusic(true);
    });

    connect(m_uiMain, &UIMain::playModeChanged, m_songManager, &SongManager::changePlayMode);

    connect(m_songManager, &SongManager::playModeChanged, m_uiMain, &UIMain::changePlayMode);

    // 播放错误提示
    connect(m_controller, &PlayerController::playbackError, this, [this](const QString &msg){
        QMessageBox::warning(nullptr, tr("Playback error"), msg);
    });
}


void AppMediator::progressSliderConnect()
{
    // 设置进度条范围
    connect(m_controller, &PlayerController::durationChanged, m_uiMain, &UIMain::setProgressSliderRange);

    // 当前进度条时长（文本）
    connect(m_controller, &PlayerController::positionChanged, m_uiMain, &UIMain::setCurDuration);

    // 播放器进度同步给进度条滑块
    connect(m_controller, &PlayerController::positionChanged, [this](qint64 position){
        if(!m_isDragging) m_uiMain->setProgressValue(position);
    });

    // 拖动开始：标记正在拖动
    connect(m_uiMain, &UIMain::sliderPressed, [this](){ m_isDragging = true;});

    // 拖动结束：标记结束拖动
    connect(m_uiMain, &UIMain::sliderReleased, [this](){
        m_isDragging = false;
        m_controller->setPlayProgress(m_uiMain->progressValue());
    });

    //播放进度同步到歌词
    connect(m_controller, &PlayerController::positionChanged, m_detailWidget, &MusicDetailWidget::onAudioPositionChanged);
}


void AppMediator::volumeSliderConnect()
{
    // 进度条和播放器音量同步
    connect(m_uiMain->volumeSlider(), &QSlider::valueChanged, m_controller, &PlayerController::setVolume);
    connect(m_controller, &PlayerController::volumeChanged, m_uiMain, &UIMain::setVolumeValue);
}

void AppMediator::pageConnect()
{
    connect(m_uiSideBar, &UISideBar::pageChanged, m_uiMain, &UIMain::switchStackedWidget);
}

void AppMediator::collectConnect()
{
    connect(m_uiMain, &UIMain::songCollected, this, &AppMediator::collectSong);

    connect(&ContextMenu::getInstance(), &ContextMenu::songCollected, this, &AppMediator::collectSong);
}

void AppMediator::searchConnect()
{
    connect(m_uiSearch->searchBar(), &QLineEdit::textChanged, m_songManager->searchModel(), &SearchProxyModel::setKeyWord);
    connect(m_uiSearch, &UISearch::songPlayRequest, m_uiMain, &UIMain::doubleClickPlay);
}

void AppMediator::playlistConnect()
{
    // 侧边栏歌单点击，更新主界面歌单信息
    connect(m_uiSideBar, &UISideBar::playlistClicked, m_uiMain, [this](const PlayListInfo &info){
        m_uiMain->setPlaylistName(info.name);
        m_uiMain->setSonglistCover(info.id, info.coverPath);
        if(m_songManager->playlistModel()->rowCount() == 0){
            m_uiMain->playlistBtn()->setDisabled(true);
        }
        else{
            m_uiMain->playlistBtn()->setDisabled(false);
        }
    });

    // 侧边栏歌单点击，更新主界面歌单歌曲列表
    connect(m_uiSideBar, &UISideBar::playlistUpdated, m_songManager->playlistModel(), &PlayListProxyModel::setAllowedSongIds);

    // 侧边栏右键播放歌单
    connect(m_uiSideBar, &UISideBar::playlistPlayed, m_uiMain->playlistBtn(), &QPushButton::click);

    // 重命名歌单
    connect(m_uiSideBar, &UISideBar::playlistNameChanged, this, [this](const QString& name, int playlist_id){
        if(DbManager::getInstance().updatePlaylistName(name, playlist_id)){
            ContextMenu::getInstance().updatePlaylistName(name, playlist_id);
            m_uiMain->setPlaylistName(name);
        }
    });
}

void AppMediator::detailWidgetConnect()
{
    connect(m_uiMain, &UIMain::detailWidgetShowed, m_detailWidget, &MusicDetailWidget::setVisible);
    connect(m_uiMain, &UIMain::detailWidgetShowed, m_uiSideBar, &UISideBar::setHidden);
    connect(m_uiMain, &UIMain::detailWidgetShowed, m_uiSearch, &UISearch::setHidden);
}

void AppMediator::contextMenuConnect()
{
    auto &contextMenu = ContextMenu::getInstance();

    connect(&contextMenu, &ContextMenu::songRemoved, this, [this](int song_id, int row){
        int pageIndex = m_uiMain->currentPage();

        auto *proxyModel = qobject_cast<QSortFilterProxyModel*>(m_uiMain->currentListView()->model());
        QModelIndex index = proxyModel->index(row, 0);

        if(pageIndex == MainPage::Collect){
            this->collectSong(false, SongManager::mapToSource(index));
        }
        else if(pageIndex == MainPage::PlayList){
            QPushButton* btn = m_uiSideBar->findSonglistBtn(m_uiSideBar->playlistSelectNumber());
            if(btn){
                PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
                if(!DbManager::getInstance().removeSongToPlaylist(info.id, song_id)){
                    return;
                }
                btn->click();
            }
        }
        else if(pageIndex == MainPage::Local){
            QMessageBox::StandardButton reply = QMessageBox::question(
                nullptr,
                "确认",
                "确定要删除这个歌曲吗?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
                );

            if(reply == QMessageBox::No || !DbManager::getInstance().removeSong(song_id)){
                return;
            }

            m_songManager->setInvalid(index);
        }

        m_songManager->setListRows(m_uiMain->geListRows(proxyModel));

        ProxyId proxyId = proxyModel->property("proxyId").value<ProxyId>();
        if(proxyId == m_songManager->proxyId() && row == m_songManager->currentIndex().row()){
            this->skipMusic(true);
        }
    });

    connect(&contextMenu, &ContextMenu::songPlayed, this, [this](int row){
        QListView *view = m_uiMain->currentListView();
        QModelIndex index = view->model()->index(row, 0);

        if(index.isValid()){
            m_uiMain->doubleClickPlay(index, true);
        }
    });

    connect(&contextMenu, &ContextMenu::progressChanged, m_songManager, &SongManager::setProgress);

    connect(&contextMenu, &ContextMenu::songAppended, m_songManager, &SongManager::appendSong);
}

void AppMediator::collectSong(bool isCollect, const QModelIndex &index)
{
    QModelIndex idx;
    if(!index.isValid()) idx = SongManager::mapToSource(m_songManager->currentIndex());
    else idx = SongManager::mapToSource(index);

    if(!idx.isValid()) return;

    if(!m_songManager->setFavorite(idx, isCollect)){
        qCWarning(mediatorLog) << (isCollect ? "收藏失败" : "取消收藏失败");
        return;
    };

    int song_id = idx.data(Roles::Id).toInt();
    if(song_id == m_songManager->currentIndex().data(Roles::Id).toInt()){
        m_uiMain->collectIconToggle(isCollect);
    }
}

void AppMediator::skipMusic(bool isNext)
{
    QModelIndex nextIndex = m_songManager->setNextIndex(isNext);

    m_uiMain->setCurrentIndex(nextIndex);
    m_uiMain->setPlayStyle(nextIndex);

    m_songManager->setPlayingStatus(nextIndex);

    m_controller->skipSong(nextIndex);

    m_detailWidget->flushDetail(nextIndex);
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
        m_songManager->setPlaylistPlayingNumber(m_uiSideBar->playlistSelectNumber());

    m_songManager->setCurrentIndex(index);
    m_songManager->setPlayingStatus(index);
    m_songManager->setListRows(m_uiMain->geListRows(proxyModel));

    m_uiMain->setCurrentIndex(index);

    m_controller->playSong(index, autoPlay); // 交给音乐控制器来真正播放歌曲

    m_detailWidget->flushDetail(index); // 刷新歌曲详情页的内容
}




