#include "appmediator.h"
#include "logging.h"
#include "image_loader_global.h"
#include "contextmenu.h"
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


    auto &contextMenu = ContextMenu::getInstance();

    connect(&contextMenu, &ContextMenu::songRemoved, this, [this](int song_id, int row){
        int pageIndex = m_uiMain->currentPage();
        if(pageIndex == Page::Collect){
            QModelIndex index = m_songManager->collectModel()->index(row, 0);
            this->collectSong(false, SongManager::mapToSource(index));
        }
        else if(pageIndex == Page::PlayList){
            QPushButton* btn = m_uiSideBar->findSonglistBtn(m_uiSideBar->playlistNumber());
            if(btn){
                PlayListInfo info = btn->property("playlist").value<PlayListInfo>();
                DbManager::getInstance().deleteSongToPlaylist(info.id, song_id);
                btn->click();
            }
        }
    });

    connect(&contextMenu, &ContextMenu::songPlayed, this, [this](int row){
        QListView *view = m_uiMain->currentListView();
        QModelIndex index = view->model()->index(row, 0);

        if(index.isValid()){
            m_uiMain->doubleClickPlay(index, true);
        }
    });
}

void AppMediator::playConnect()
{
    // 设置播放按钮Icon
    connect(m_controller->mediaPlayer(), &QMediaPlayer::playbackStateChanged, m_uiMain, &UIMain::setPlayBtnIcon);

    // 双击播放
    connect(m_uiMain, &UIMain::songPlayed, this, &AppMediator::playMusic);

    // 手动控制播放/停止
    connect(m_uiMain, &UIMain::songPlayedOrPaused, m_controller, &PlayerController::playOrPause);

    // 下/上一首
    connect(m_uiMain, &UIMain::songSkipped, this, &AppMediator::skipMusic);

    // 播放结束自动下一首
    connect(m_controller->mediaPlayer(), &QMediaPlayer::mediaStatusChanged, [this](QMediaPlayer::MediaStatus status){
        if(status == QMediaPlayer::MediaStatus::EndOfMedia) skipMusic(true);
    });

    connect(m_uiMain, &UIMain::modeChanged, m_songManager, &SongManager::changePlayMode);

    connect(m_songManager, &SongManager::modeChanged, m_uiMain, &UIMain::changePlayMode);

    // 播放错误提示
    connect(m_controller, &PlayerController::playbackError, this, [this](const QString &msg){
        QMessageBox::warning(nullptr, tr("Playback error"), msg);
    });
}


void AppMediator::progressSliderConnect()
{
    // 设置进度条范围
    connect(m_controller->mediaPlayer(), &QMediaPlayer::durationChanged, m_uiMain, &UIMain::setProgressSliderRange);

    // 当前进度条时长（文本）
    connect(m_controller->mediaPlayer(), &QMediaPlayer::positionChanged, m_uiMain, &UIMain::setCurDuration);

    // 播放器进度同步给进度条滑块
    connect(m_controller->mediaPlayer(), &QMediaPlayer::positionChanged, [this](qint64 position){
        if(!m_isDragging) m_uiMain->setProgressValue(position);
    });

    // 拖动开始：标记正在拖动
    connect(m_uiMain->progressSlider(), &QSlider::sliderPressed, [this](){ m_isDragging = true;});

    // 拖动结束：标记结束拖动
    connect(m_uiMain->progressSlider(), &QSlider::sliderReleased, [this](){
        m_isDragging = false;
        m_controller->setPlayProgress(m_uiMain->progressValue());
    });

    //播放进度同步到歌词
    connect(m_controller->mediaPlayer(), &QMediaPlayer::positionChanged, m_detailWidget, &MusicDetailWidget::onAudioPositionChanged);
}


void AppMediator::volumeSliderConnect()
{
    // 进度条和播放器音量同步
    connect(m_uiMain->volumeSlider(), &QSlider::valueChanged, m_controller, &PlayerController::setVolume);
    connect(m_controller->audioOutput(), &QAudioOutput::volumeChanged, m_uiMain, &UIMain::setVolumeValue);
}

void AppMediator::pageConnect()
{
    connect(m_uiSideBar, &UISideBar::pageChanged, m_uiMain, &UIMain::switchStackedWidget);
}

void AppMediator::collectConnect()
{
    // 收藏歌曲
    connect(m_uiMain, &UIMain::collected, [this](const QModelIndex &index){
        collectSong(true, index);
    });

    // 取消歌曲收藏
    connect(m_uiMain, &UIMain::cancelCollected, [this](const QModelIndex &index){
        collectSong(false, index);
    });
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
        m_uiMain->setSonglistCover(info.cover);
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
    connect(m_uiMain, &UIMain::showDetailWidget, m_detailWidget, &MusicDetailWidget::setVisible);
    connect(m_uiMain, &UIMain::showDetailWidget, m_uiSideBar, &UISideBar::setHidden);
    connect(m_uiMain, &UIMain::showDetailWidget, m_uiSearch, &UISearch::setHidden);
}

void AppMediator::collectSong(bool isCollect, const QModelIndex &index)
{
    QModelIndex idx;
    if(!index.isValid()) idx = SongManager::mapToSource(m_songManager->currentIndex());
    else idx = SongManager::mapToSource(index);

    if(!idx.isValid()) return;

    int song_id = idx.data(Roles::Id).toInt();

    bool dbOk = false;
    if(isCollect) dbOk = DbManager::getInstance().collectSong(song_id);
    else  dbOk = DbManager::getInstance().disCollectSong(song_id);

    if(!dbOk){
        QMessageBox::warning(m_uiMain, "操作失败", isCollect ? "收藏失败，请重试" : "取消收藏失败，请重试");
        return;
    }

    if(!m_songManager->setFavorite(idx, isCollect)){
        qCWarning(mediatorLog) << (isCollect ? "收藏失败" : "取消收藏失败");
        return;
    };


    if(song_id == m_songManager->currentIndex().data(Roles::Id).toInt()){
        m_uiMain->collectIconToggle(isCollect);
    }
}

void AppMediator::skipMusic(bool isNext)
{
    QModelIndex nextIndex = m_songManager->setNextIndex(isNext); // 设置下一首歌曲的索引
    if(!nextIndex.isValid()) return;

    m_uiMain->setCurrentIndex(nextIndex); // 设置上/下一首歌曲为选中状态
    m_uiMain->setPlayStyle(nextIndex); // 设置上/下一首的播放样式

    m_songManager->setPlayingStatus(nextIndex);
    m_songManager->setCurrentIndex(nextIndex);

    m_controller->skipMusic(nextIndex); // 交给音乐控制器来真正播放上/下一首歌曲

    m_detailWidget->flushDetail(nextIndex); // 刷新全屏播放页
}

void AppMediator::playMusic(const QModelIndex &index, bool autoPlay)
{
    if(!index.isValid()) return;

    const QSortFilterProxyModel *proxyModel = qobject_cast<const QSortFilterProxyModel*>(index.model());
    m_songManager->setListRows(m_uiMain->getPlaylistRows(proxyModel)); //  设置当前歌曲所在的列表为播放列表

    ProxyId id = proxyModel->property("proxyId").value<ProxyId>();
    Page page = m_songManager->pageOfProxy(id);

    if(page != m_songManager->playPage()) m_songManager->setPlayPage(page);;

    if(id == ProxyId::SongList){
        m_songManager->setPalylistLastNumber(m_uiSideBar->playlistNumber());
    }

    m_uiMain->setCurrentIndex(index);

    m_songManager->setCurrentIndex(index);
    m_songManager->setPlayingStatus(index);

    m_controller->playMusic(index, autoPlay); // 交给音乐控制器来真正播放歌曲
    m_detailWidget->flushDetail(m_songManager->currentIndex()); // 刷新歌曲详情页的内容
}




