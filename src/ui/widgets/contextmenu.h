#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "song_info.h"
#include "playlist_info.h"
#include "http_request.h"
#include "downloader.h"
#include <QObject>
#include <QMenu>
#include <QPoint>
#include <QListView>
#include <functional>

// 右键菜单管理（单例模式）

class ContextMenu : public QWidget
{
    Q_OBJECT
public:
    // 禁用拷贝构造、赋值
    ContextMenu(const ContextMenu&) = delete;
    ContextMenu& operator=(const ContextMenu&) = delete;

    // 返回ContextMenu单例对象
    static ContextMenu& getInstance();

    using QueryPlaylistsFunc = std::function<QSet<int>(int)>;
    using InsertSongFunc = std::function<bool(int, int)>;
    using SongCountFunc = std::function<int(int)>;
    using UpdateCoverFunc = std::function<bool(const QString, int, int)>;

    void setPlaylistDataSource(QueryPlaylistsFunc queryPlaylists,
                               InsertSongFunc insertSongs,
                               SongCountFunc songCount,
                               UpdateCoverFunc updateCover);


    // 显示右键菜单
    void show(QListView *view, const QPoint &pos);

    // 新右键菜单中的歌单名称
    void updatePlaylistName(const QString &name, int palylist_id);

    // 设置QAction图标
    void setActionIcon(QAction *action, const QString &filePath,int cover_id);

    // 批量添加歌单
    void addPlaylists(const QList<PlayListInfo> &list);

    // 置歌单的启用性
    void setPlaylistEnabled(const QSet<int> &set);

signals:
    // 右键菜单中的歌单封面图片更新信号
    void playlistCoverUpdated(int playlist_id, int song_id, const QString& path);

    // 歌曲列表封面图片更新信号
    void songlistCoverUpdated(int song_id, const QString& path);

    // 歌曲删除信号
    void songRemoved(int song_id, int row);

    // 歌曲播放信号
    void songPlayed(int row);

    // 歌曲收藏信号
    void songCollected(bool isCollect, const QModelIndex &index);

    // 进度值更改信号
    void downloadProgressChanged(const QModelIndex &index, int progressValue);

    // 添加歌曲信号
    void songAdded(SongInfo song);

public slots:
    // 添加歌曲到歌单中
    bool addSongToPlaylist(QAction* action);

    // 移动歌曲到指定歌单中
    void moveSongToPlaylist(QAction* action); // 移动歌曲到被的歌单中

    // 添加歌单
    void addPlaylist(const PlayListInfo &info);

    // 删除歌单
    void removePlaylist(int playlist_id); // 删除右键菜单中的歌单
private:
    ContextMenu();
    ~ContextMenu();

    void init(); // 初始化

    // 为右键菜单添加歌单
    QAction* createAction(const PlayListInfo &info);

    // 同步两个QAction的显示状态
    void syncConnect(QAction *src, QAction *dist);

    // 设置移动菜单的可见性
    void setMoveMenuVisible(QListView *view);

    // 设置下载歌曲作的可见性
    void setDownloadActionVisible(QListView *view);

    // 设置收藏歌曲操作的可见性
    void setLoveActionIcon(bool isFavo);

    // 设置删除歌曲操作的可见性
    void setDelActionVisible(QListView *view);

    // 下载歌曲
    void downloadSong(QModelIndex index);

    QMenu *m_contextMenu = nullptr;
    QMenu *m_addToMenu = nullptr;
    QMenu *m_moveToMenu = nullptr;

    QAction *m_playAction = nullptr;
    QAction *m_loveAction = nullptr;
    QAction *m_delAction = nullptr;
    QAction *m_downloadAction = nullptr;

    QModelIndex m_curIndex = QModelIndex();

    bool m_syncing = false; // 同步标识，用于syncConnect函数

    Downloader m_downloader;

    QueryPlaylistsFunc m_queryPlaylists;
    InsertSongFunc m_insertSongs;
    SongCountFunc m_songCount;
    UpdateCoverFunc m_updateCover;
};

#endif // CONTEXTMENU_H
