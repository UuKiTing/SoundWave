#ifndef PLAYLIST_REPOSITORY_H
#define PLAYLIST_REPOSITORY_H

#include "playlist_info.h"

class DbHelper;

class PlaylistRepository
{
public:
    PlaylistRepository();
    ~PlaylistRepository();

    // 创建歌单
    PlayListInfo createPlaylist(const QString &name);

    // 插入歌曲到歌单中
    bool insertSongToPlaylist(int palylist_id, int song_id);

    // 删除歌单中的歌曲
    bool removeSongToPlaylist(int playlist_id, int song_id);

    // 更新歌单封面图片
    bool updatePlaylistCover(const QString &path, int playlist_id, int cover_id);

    // 删除歌单
    bool deletePlaylist(int playlist_id);

    // 更新歌单名称
    bool updatePlaylistName(const QString &name, int playlist_id);

    // 查询所有歌单
    QList<PlayListInfo> queryPlaylists();

    // 查询某一歌单
    PlayListInfo queryOneOfPlaylists(const QString &name);

    // 查询某歌单下的所以歌曲id
    QSet<int> queryPlaylistId(int playlist_id);

    // 查询某歌单在的歌曲数量
    int songCountInPlaylist(int playlist_id);

    // 查找某歌曲所在的歌单
    QSet<int> findPlaylistsBySong(int song_id);

private:
    DbHelper *m_dbHelper;
};

#endif // PLAYLIST_REPOSITORY_H
