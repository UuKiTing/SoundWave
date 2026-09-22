#include "dbmanager.h"
#include "logging.h"
#include "path_manager.h"
#include <QStandardPaths>
#include <QSqlRecord>
#include <QCoreApplication>
#include <QDir>
#include <QSqlTableModel>


DbManager::DbManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(Paths::databasePath());

    if(!m_db.open()){
        qCCritical(dbLog) << "数据库打开失败: " << m_db.lastError().text();
        m_isValid = false;
        return;
    }

    QSqlQuery query(m_db);

    if(!query.exec("PRAGMA foreign_keys = ON")){ // SQLite默认不启用外键约束
        qCCritical(dbLog) << "启用外键约束失败" << m_db.lastError().text();
    }
    else{
        query.exec("PRAGMA foreign_keys");
        if(query.next() && query.value(0).toInt() != 1){
            qCCritical(dbLog) << "外键约束未生效！";
        }
    }

    m_isValid = createSongTable() &&
                createCollectionTable() &&
                createPlaylistsTable() &&
                createPlaylistSongsTable();

    if(!m_isValid){
        qCCritical(dbLog) << "数据库初始化失败！";
    }
}

DbManager &DbManager::getInstance()
{
    static DbManager db;
    return db;
}


QList<SongInfo> DbManager::loadSongs()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM songs LEFT JOIN (SELECT song_id FROM collection) as coll "
                  "ON coll.song_id = songs.id;");

    if(!query.exec()){
        return {};
    }

    QList<SongInfo> list;

    while(query.next()){
        SongInfo info;
        info.id = query.record().value("id").toInt();
        info.title = query.record().value("title").toString();
        info.artist = query.record().value("artist").toString();
        info.duration = query.record().value("duration").toInt();
        info.durationString = toDurationString(info.duration);
        info.audioPath = query.record().value("audio_path").toString();
        info.coverPath = query.record().value("cover_path").toString();
        info.lyricsPath = query.record().value("lyrics_path").toString();
        info.isFavorite = !query.record().value("song_id").isNull();

        list.append(info);
    }

    return list;
}

bool DbManager::isValid()
{
    return m_isValid;
}

bool DbManager::appendSong(const SongInfo &info)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO songs VALUES(:id, :title, :artist, :duration, :audioPath, :coverPath, :lyricsPath)");

    query.bindValue(":id", info.id);
    query.bindValue(":title", info.title);
    query.bindValue(":artist", info.artist);
    query.bindValue(":duration", info.duration);
    query.bindValue(":audioPath", info.audioPath);
    query.bindValue(":coverPath", info.coverPath);
    query.bindValue(":lyricsPath", info.lyricsPath);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::removeSong(int song_id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM songs WHERE id = :song_id");
    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::collectSong(int song_id)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO collection (song_id)VALUES(:song_id)");

    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::disCollectSong(int song_id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM collection WHERE song_id = :song_id");
    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

QList<int> DbManager::queryCollectSongs()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT song_id FROM collection");

    if(!query.exec()){
        return {};
    }

    QList<int> list;

    while(query.next()) list.append(query.value(0).toInt());

    return list;
}

PlayListInfo DbManager::createPlaylist(const QString &name)
{
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO playlists (name, cover_path) VALUES(:name, :cover_path);");
    query.bindValue(":name", name);
    query.bindValue(":cover_path", ":/icon/cover.png");

    if(!query.exec()){
        return {};
    }

    return queryOneOfPlaylists(name);
}

QList<PlayListInfo> DbManager::queryPlaylists()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM playlists");

    QList<PlayListInfo> list;

    if(!query.exec()){
        return list;
    }


    while(query.next()){
        PlayListInfo info;

        info.id = query.value("id").toInt();
        info.name = query.value("name").toString();
        info.coverPath = query.value("cover_path").toString();

        list.append(info);
    }

    return list;
}

PlayListInfo DbManager::queryOneOfPlaylists(const QString &name)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, cover_path FROM playlists WHERE name = :name;");
    query.bindValue(":name", name);

    PlayListInfo info;

    if(!query.exec()){
        return info;
    }

    query.next();

    info.id = query.record().value("id").toInt();
    info.name = query.record().value("name").toString();
    info.coverPath = query.record().value("cover_path").toString();

    return info;
}

QSet<int> DbManager::queryPlaylistId(int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT song_id FROM playlist_songs  WHERE playlist_id = :playlist_id ORDER BY sort_order DESC
)");

    query.bindValue(":playlist_id", playlist_id);

    QSet<int> set;

    if(!query.exec()){
        return {};
    }

    while(query.next())
        set.insert(query.record().value("song_id").toInt());

    return set;
}

bool DbManager::insertSongToPlaylist(int palylist_id, int song_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO playlist_songs (playlist_id, song_id, sort_order)
        VALUES (
            :playlist_id,
            :song_id,
            COALESCE((SELECT MAX(sort_order) FROM playlist_songs WHERE playlist_id = :playlist_id), 0) + 1
        );
    )");

    query.bindValue(":playlist_id", palylist_id);
    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::removeSongToPlaylist(int playlist_id, int song_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        DELETE FROM playlist_songs WHERE playlist_id = :playlist_id AND song_id = :song_id
    )");

    query.bindValue(":playlist_id", playlist_id);
    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::updatePlaylistCover(const QString &path, int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE playlists SET cover_path = :cover_path WHERE id = :playlist_id;");
    query.bindValue(":cover_path", path);
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

int DbManager::songCountInPlaylist(int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT count(*)
        FROM playlist_songs
        WHERE playlist_id = :playlist_id
)");

    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        return -1;
    }

    return query.next() ? query.value(0).toInt() : -1;
}

bool DbManager::deletePlaylist(int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM playlists WHERE id = :playlist_id;");
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

bool DbManager::updatePlaylistName(const QString &name, int playlist_id)
{

    QSqlQuery query(m_db);
    query.prepare("UPDATE playlists SET name = :name WHERE id = :playlist_id;");
    query.bindValue(":name", name);
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        return false;
    }

    return true;
}

QSet<int> DbManager::findPlaylistsBySong(int song_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT playlist_id FROM playlist_songs WHERE :song_id = song_id
)");

    query.bindValue(":song_id", song_id);

    QSet<int> set;

    if(!query.exec()){
        return {};
    }


    while(query.next())
        set.insert(query.record().value("playlist_id").toInt());

    return set;
}

bool DbManager::createSongTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS songs (
            id INTEGER PRIMARY KEY,
            title TEXT,
            artist TEXT,
            duration INTEGER,
            audio_path TEXT,
            cover_path TEXT,
            lyrics_path text
            );
    )";

    if(!query.exec(sql)){
        return false;
    }

    return true;
}

bool DbManager::createCollectionTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS collection (
            id INTEGER PRIMARY KEY,
            song_id INTEGER NOT NULL UNIQUE
            );
    )";


    if(!query.exec(sql)){
        return false;
    }

    return true;
}

bool DbManager::createPlaylistsTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS playlists (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            cover_path TEXT,
            UNIQUE(name)
            );
    )";


    if(!query.exec(sql)){
        return false;
    }

    return true;
}

bool DbManager::createPlaylistSongsTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS playlist_songs (
            id INTEGER PRIMARY KEY,
            playlist_id INTEGER NOT NULL,
            song_id INTEGER NOT NULL,
            sort_order INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (playlist_id) REFERENCES playlists(id) ON DELETE CASCADE,
            UNIQUE(playlist_id, song_id)
        );
    )";

    if(!query.exec(sql)){
        return false;
    }

    return true;
}


DbManager::~DbManager()
{
    if(m_db.isOpen()){
        m_db.close();
    }
}
