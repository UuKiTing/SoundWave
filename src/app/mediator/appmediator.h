#ifndef APPMEDIATOR_H
#define APPMEDIATOR_H

#include "uimain.h"
#include "uisearch.h"
#include "uisidebar.h"
#include "player_controller.h"
#include "song_manager.h"
#include "music_detail_widget.h"
#include "song_repository.h"
#include "playlist_repository.h"
#include <QObject>
#include <QModelIndex>

class MainWindow;

class AppMediator : public QObject
{
    Q_OBJECT
public:
    explicit AppMediator(
        PlayerController *controller,
        SongManager *songManager,
        MainWindow *mainWindow,
        QObject *parent = nullptr);

signals:

private slots:
    void skipMusic(bool isNext); // isNext：为true时是下一首歌曲，反之上一首歌曲
    void playMusic(const QModelIndex &index, bool autoPlay); // 播放歌曲
    void removeSong(int song_id, int row);

private:
    void connectSignals();
    void playConnect(); // 播放功能
    void progressSliderConnect(); // 进度条
    void volumeSliderConnect(); //  音量
    void pageConnect(); // 页面切换
    void collectConnect(); // 音乐收藏
    void searchConnect(); // 搜索框
    void playlistConnect(); // 歌单
    void detailWidgetConnect(); // 详情页
    void contextMenuConnect(); // 上下文菜单

    void collectSong(bool isCollect, const QModelIndex &index = QModelIndex()); // 收藏音乐

    void handleLocalSongRemoved(int song_id, int row);
    void handleCollectSongRemoved(int song_id, int row);
    void handlePlaylistSongRemoved(int song_id, int row);

    void updatePlaybackAfterRemoved(int removedRow);


    PlayerController *m_controller{}; ///< 播放控制器
    SongManager *m_songManager{}; ///< 音乐列表管理器
    MainWindow *m_mainWindow{}; ///< 主窗口

    SongRepository m_songRepo;
    PlaylistRepository m_playlistRepo;
};

#endif // APPMEDIATOR_H
