#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "global.h"
#include <QObject>
#include <QMenu>
#include <QPoint>
#include <QListView>

class ContextMenu : public QWidget
{
    Q_OBJECT
public:
    ContextMenu(const ContextMenu&) = delete;
    ContextMenu operator=(const ContextMenu&) = delete;

    static ContextMenu& getInstance();

    void show(QListView *view, const QPoint &pos); // 显示右键菜单

    void updatePlaylistName(const QString &name, int palylist_id);

signals:
    void playlistCoverUpdated(int playlist_id, const QString& path);
    void songlistCoverUpdated(const QString& path);
    void songRemoved(int song_id, int row);
    void songPlayed(int row);

public slots:
    bool addSongToPlaylist(QAction* action); // 添加音乐到歌单中
    void moveSongToPlaylist(QAction* action); // 移动歌曲到被的歌单中
    void addPlaylist(const PlayListInfo &info);
    void removePlaylist(int playlist_id); // 删除右键菜单中的歌单

private:
    ContextMenu();
    ~ContextMenu();

    init(); // 初始化
    QAction* createAction(const PlayListInfo &info); // 往右键菜单中添加歌单
    void syncConnect(QAction *src, QAction *dist);
    void updatePlaylistMenu();
    void setMoveMenuVisible(QListView *view);

    QMenu *m_contextMenu{}; // 右键菜单
    QMenu *m_addToMenu{};
    QMenu *m_moveToMenu{};

    QAction *m_play;
    QAction *m_love;
    QAction *m_del;

    bool m_syncing = false;
};

#endif // CONTEXTMENU_H
