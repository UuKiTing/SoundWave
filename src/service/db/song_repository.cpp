#include "song_repository.h"
#include "dbhelper.h"

SongRepository::SongRepository(): m_dbHelper(new DbHelper) {}

SongRepository::~SongRepository()
{
    delete m_dbHelper;
}

QList<SongInfo> SongRepository::loadSongs()
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QString sql = "SELECT * FROM songs LEFT JOIN (SELECT song_id FROM collection) as coll ON coll.song_id = songs.id;";

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db, sql);

    QList<SongInfo> list;

    for(const auto& record : results){
        SongInfo info;
        info.id = record.value("id").toInt();
        info.title = record.value("title").toString();
        info.artist = record.value("artist").toString();
        info.duration = record.value("duration").toInt();
        info.durationString = toDurationString(info.duration);
        info.audioPath = record.value("audio_path").toString();
        info.coverPath = record.value("cover_path").toString();
        info.lyricsPath = record.value("lyrics_path").toString();
        info.isFavorite = !record.value("song_id").isNull();

        list.append(info);
    }

    return list;
}

bool SongRepository::appendSong(const SongInfo &info)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->insert(db,
                            "songs",
                            {"id", "title", "artist", "duration", "audio_path", "cover_path", "lyrics_path"},
                            {info.id, info.title, info.artist, info.duration, info.audioPath, info.coverPath, info.lyricsPath}
                            );
}

bool SongRepository::removeSong(int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->remove(db,
                            "songs",
                            {{"id",song_id}});
}

bool SongRepository::collectSong(int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->insert(db,
                            "collection",
                            {"song_id"},
                            {song_id});
}

bool SongRepository::disCollectSong(int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->remove(db,
                            "collection",
                            {{"song_id", song_id}});
}

QList<int> SongRepository::queryCollectSongs()
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db, "collection", {"song_id"});

    QList<int> list;
    for(const auto &record : results){
        list.append(record["song_id"].toInt());
    }

    return list;
}
