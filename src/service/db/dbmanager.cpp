#include "dbmanager.h"
#include "logging.h"
#include "path_manager.h"
#include <QStandardPaths>
#include <QSqlRecord>
#include <QCoreApplication>
#include <QDir>
#include <QSqlTableModel>
#include <QThread>

DbManager::DbManager()
{
    QSqlDatabase db = getConnection();

    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON")){ // SQLite默认不启用外键约束
        qCCritical(dbLog) << "启用外键约束失败" << db.lastError().text();
        m_isValid = false;
    }
    else{
        query.exec("PRAGMA foreign_keys");
        if(query.next() && query.value(0).toInt() != 1){
            qCCritical(dbLog) << "外键约束未生效！";
            m_isValid = false;
        }
    }

    m_isValid = createSongTable(db) &&
                createCollectionTable(db) &&
                createPlaylistsTable(db) &&
                createPlaylistSongsTable(db) &&
                m_isValid;

    if(!m_isValid){
        qCCritical(dbLog) << "数据库初始化失败！";
    }
}

DbManager &DbManager::getInstance()
{
    static DbManager db;
    return db;
}

QSqlDatabase DbManager::getConnection()
{
    QMutexLocker locker(&m_mutex);
    QString name = QString("db_%1").arg((quintptr)QThread::currentThreadId());
    if(!QSqlDatabase::contains(name)){
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
        db.setDatabaseName(Paths::databasePath());
        db.open();
    }

    return QSqlDatabase::database(name);
}

bool DbManager::isValid()
{
    return m_isValid;
}

bool DbManager::createSongTable(QSqlDatabase db)
{
    QSqlQuery query(db);
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

bool DbManager::createCollectionTable(QSqlDatabase db)
{
    QSqlQuery query(db);
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

bool DbManager::createPlaylistsTable(QSqlDatabase db)
{
    QSqlQuery query(db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS playlists (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            cover_path TEXT,
            cover_id INTEGER,
            UNIQUE(name)
            );
    )";

    if(!query.exec(sql)){
        return false;
    }

    return true;
}

bool DbManager::createPlaylistSongsTable(QSqlDatabase db)
{
    QSqlQuery query(db);
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
}
