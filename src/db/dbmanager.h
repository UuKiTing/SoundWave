#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "global.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QModelIndex>

class DbManager
{
public:
    static DbManager& getInstance();

    QList<SongInfo> loadSongs();
    bool isValid();

    bool appendMusicData(const SongInfo &info); // 添加新歌曲
    bool collectSong(int song_id); // 收藏歌曲
    bool disCollectSong(int song_id); // 取消歌曲收藏

    PlayListInfo createPlaylist(int user_id, const QString &name); // 创建一个歌单
    QList<PlayListInfo> queryPlaylists(int user_id); // 查询该用户的所有歌单
    PlayListInfo  queryOneOfPlaylists(int user_id, const QString &name); // 查询该用户的某个歌单
    QSet<int> queryPlaylistId(int playlist_id); // 查询该歌单下的所有歌曲id
    bool insertSongToPlaylist(int palylist_id, int song_id); // 添加歌曲到歌单
    bool deleteSongToPlaylist(int playlist_id, int song_id); // 删除歌单中的歌曲
    bool updatePlaylistCover(const QString &path, int playlist_id); // 更新歌单封面
    int songCountInPlaylist(int playlist_id); // 查询歌单下的歌曲数量
    bool deletePlaylist(int playlist_id); // 删除歌单
    bool updatePlaylistName(const QString &name, int playlist_id);

    QSet<int> findPlaylistsBySong(int song_id);

    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

private:
    void createUsersTable();
    void createSongTable();
    void createCollectionTable();
    void createPlaylistsTable();
    void createPlaylistSongsTable();

    explicit DbManager();
    ~DbManager();

    QSqlDatabase m_db;
    bool m_isValid = false;
};

#endif // DBMANAGER_H
