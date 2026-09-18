#include "dbmanager.h"
#include "logging.h"
#include <QStandardPaths>
#include <QSqlRecord>
#include <QCoreApplication>
#include <QDir>
#include <QSqlTableModel>


DbManager::DbManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    // WARNING: 当前数据库放在了程序同级目录，发布前必须迁移到 QStandardPaths 标准路径下
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    m_db.setDatabaseName(dir.filePath("music.db"));

    // QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // QDir().mkpath(dbPath);
    // m_db.setDatabaseName(QDir(dbPath).filePath("music.db"));

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


    m_isValid = true;

    createSongTable();
    createCollectionTable();
    createPlaylistsTable();
    createPlaylistSongsTable();
}


DbManager &DbManager::getInstance()
{
    static DbManager db;
    return db;
}


QList<SongInfo> DbManager::loadSongs()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM songs LEFT JOIN (SELECT music_id FROM collection) as coll "
                  "ON coll.music_id = songs.id;");

    if(!query.exec()){
        qCWarning(dbLog) << "加载歌曲信息失败：" << query.lastError().text();
        return {};
    }

    QList<SongInfo> list;

    while(query.next()){
        SongInfo info;
        info.id = query.record().value("id").toInt();
        info.remoteId = -1;
        info.source = SongSource::Local;
        info.title = query.record().value("title").toString();
        info.artist = query.record().value("artist").toString();
        info.duration = query.record().value("duration").toInt();
        info.durationString = toDurationString(info.duration);
        info.filePath = query.record().value("filePath").toString();
        info.coverPath = query.record().value("cover").toString();
        info.lyricsPath = query.record().value("lyrics").toString();
        info.isFavo = !query.record().value("music_id").isNull();
        info.isPlaying = false;

        list.append(info);
    }

    return list;
}

bool DbManager::isValid()
{
    return m_isValid;
}


bool DbManager::appendMusicData(const SongInfo &info)
{
    m_db.transaction(); // 开启事务
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO songs VALUES(:id, :title, :artist, :duration, :filePath, :cover, :lyrics)");
    query.bindValue(":id", info.id);
    query.bindValue(":title", info.title);
    query.bindValue(":artist", info.artist);
    query.bindValue(":duration", info.duration);
    query.bindValue(":filePath", info.filePath);
    query.bindValue(":cover", info.coverPath);
    query.bindValue(":lyrics", info.lyricsPath);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "插入数据到歌曲表中失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::collectSong(int song_id)
{
    m_db.transaction(); // 开启事务
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO collection (music_id)VALUES(:music_id)");
    query.bindValue(":music_id", song_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "收藏失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::disCollectSong(int song_id)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM collection WHERE music_id = :music_id");
    query.bindValue(":music_id", song_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "取消收藏失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }
    return true;
}

QList<int> DbManager::queryColletSongs()
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("SELECT music_id FROM collection");

    if(!query.exec()){
        qCWarning(dbLog) << "查找收藏歌曲失败：" << query.lastError().text();
        return {};
    }

    QList<int> list;

    while(query.next()) list.append(query.value(0).toInt());

    return list;
}

PlayListInfo DbManager::createPlaylist(int user_id, const QString &name)
{
    m_db.transaction(); // 开启事务
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO playlists (name, cover_path) VALUES(:name, :cover_path);");
    query.bindValue(":name", name);
    query.bindValue(":cover_path", ":/icon/cover.png");

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "创建歌单失败：" << query.lastError().text();
        return {};
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return {};
    }

    return queryOneOfPlaylists(user_id, name);
}

QList<PlayListInfo> DbManager::queryPlaylists()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM playlists");

    if(!query.exec()){
        qCWarning(dbLog) << "查询所有歌单失败：" << query.lastError().text();
        return {};
    }

    QList<PlayListInfo> list;

    while(query.next()){
        PlayListInfo info;

        info.id = query.record().value("id").toInt();
        info.name = query.record().value("name").toString();
        info.cover = query.record().value("cover_path").toString();

        list.append(info);
    }

    return list;
}

PlayListInfo DbManager::queryOneOfPlaylists(int user_id, const QString &name)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM playlists WHERE name = :name;");
    query.bindValue(":name", name);

    PlayListInfo info;

    if(!query.exec()){
        qCWarning(dbLog) << "歌单查找失败：" << query.lastError().text();
        return info;
    }

    query.next();

    info.id = query.record().value("id").toInt();
    info.name = query.record().value("name").toString();
    info.cover = query.record().value("cover_path").toString();

    return info;
}

QSet<int> DbManager::queryPlaylistId(int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT * FROM playlist_songs  WHERE playlist_id = :playlist_id ORDER BY sort_order DESC
)");

    query.bindValue(":playlist_id", playlist_id);

    QSet<int> set;

    if(!query.exec()){
        qCWarning(dbLog) << "查询歌单列表id失败：" << query.lastError().text();
        return {};
    }

    while(query.next())
        set.insert(query.record().value("id").toInt());

    return set;
}

bool DbManager::insertSongToPlaylist(int palylist_id, int song_id)
{
    m_db.transaction();
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
        m_db.rollback();
        qCWarning(dbLog) << "插入歌曲到歌单列表中失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::deleteSongToPlaylist(int playlist_id, int song_id)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare(R"(
    DELETE FROM playlist_songs WHERE playlist_id = :playlist_id AND song_id = :song_id
)");

    query.bindValue(":playlist_id", playlist_id);
    query.bindValue(":song_id", song_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "从歌单中删除歌曲中失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::updatePlaylistCover(const QString &path, int playlist_id)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("UPDATE playlists SET cover_path = :cover_path WHERE id = :playlist_id;");
    query.bindValue(":cover_path", path);
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "更新歌单封面失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

int DbManager::songCountInPlaylist(int playlist_id)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT count(*)
        FROM songs s
        JOIN playlist_songs ps ON s.id = ps.song_id
        WHERE ps.playlist_id = :playlist_id;
)");

    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        qCWarning(dbLog) << "查询歌单歌曲数量失败:" << query.lastError().text();
        return -1;
    }

    return query.next() ? query.value(0).toInt() : -1;
}

bool DbManager::deletePlaylist(int playlist_id)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM playlists WHERE id = :playlist_id;");
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "删除歌单失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
        return false;
    }

    return true;
}

bool DbManager::updatePlaylistName(const QString &name, int playlist_id)
{
    m_db.transaction();
    QSqlQuery query(m_db);
    query.prepare("UPDATE playlists SET name = :name WHERE id = :playlist_id;");
    query.bindValue(":name", name);
    query.bindValue(":playlist_id", playlist_id);

    if(!query.exec()){
        m_db.rollback();
        qCWarning(dbLog) << "更新歌单封面失败：" << query.lastError().text();
        return false;
    }

    if(!m_db.commit()){
        m_db.rollback();
        qCWarning(dbLog) << "事务提交失败：" << query.lastError().text();
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
        qCWarning(dbLog) << "查询歌曲所在的歌单有哪些失败！" << query.lastError().text();
        return {};
    }


    while(query.next())
        set.insert(query.record().value("playlist_id").toInt());

    return set;
}

void DbManager::createSongTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS songs (
            id INTEGER PRIMARY KEY,
            title TEXT,
            artist TEXT,
            duration INTEGER,
            filePath TEXT,
            cover TEXT,
            lyrics text
            );
    )";

    if(!query.exec(sql)){
        qCWarning(dbLog) << "创建 songs 表失败:" << query.lastError().text();
        return;
    }
}

void DbManager::createCollectionTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS collection (
            id INTEGER PRIMARY KEY,
            music_id INTEGER NOT NULL UNIQUE
            );
    )";


    if(!query.exec(sql)){
        qCWarning(dbLog) << "创建 collection 表失败:" << query.lastError().text();
        return;
    }
}

void DbManager::createPlaylistsTable()
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
        qCWarning(dbLog) << "创建 playlists 表失败:" << query.lastError().text();
        return;
    }
}

void DbManager::createPlaylistSongsTable()
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
        qCWarning(dbLog) << "创建 playlist_songs 表失败:" << query.lastError().text();
        return;
    }
}


DbManager::~DbManager()
{
    if(m_db.isOpen()){
        m_db.close();
    }
}

