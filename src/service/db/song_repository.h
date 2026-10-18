#ifndef SONG_REPOSITORY_H
#define SONG_REPOSITORY_H

#include "dbmanager.h"

class DbHelper;

class SongRepository
{
public:
    SongRepository();
    ~SongRepository();

    // 从数据库中加载本地歌曲数据
    QList<SongInfo> loadSongs();

    // 添加新的歌曲到数据库中
    bool appendSong(const SongInfo &info);

    // 除歌曲
    bool removeSong(int song_id);

    // 收藏歌曲
    bool collectSong(int song_id);

    // 取消歌曲收藏
    bool disCollectSong(int song_id);

    // 查询收藏的歌曲有哪些
    QList<int> queryCollectSongs();

private:
    DbHelper *m_dbHelper;
};

#endif // SONG_REPOSITORY_H
