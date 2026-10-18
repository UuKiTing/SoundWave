#include "playlist_repository.h"
#include "dbhelper.h"
#include "dbmanager.h"
#include <QSqlDatabase>

PlaylistRepository::PlaylistRepository():  m_dbHelper(new DbHelper){}

PlaylistRepository::~PlaylistRepository()
{
    delete m_dbHelper;
}

PlayListInfo PlaylistRepository::createPlaylist(const QString &name)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    int res = m_dbHelper->insert(db,
                               "playlists",
                               {"name", "cover_path", "cover_id"},
                               {name, "", -1});

    if(!res) return {};

    return queryOneOfPlaylists(name);
}

bool PlaylistRepository::insertSongToPlaylist(int playlist_id, int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QueryOptions opts;
    opts.conds = {{"playlist_id", playlist_id}};

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db, "playlist_songs", {"sort_order"}, opts);

    int maxOrder = -1;
    for(const auto &record : results){
        int order = record["sort_order"].toInt();
        if(order > maxOrder) maxOrder = order;
    }

    return m_dbHelper->insert(db,
                           "playlist_songs",
                           {"playlist_id", "song_id", "sort_order"},
                           {playlist_id, song_id, maxOrder + 1});
}

bool PlaylistRepository::removeSongToPlaylist(int playlist_id, int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->remove(db,
                            "playlist_songs",
                            {{"playlist_id", playlist_id}, {"song_id", song_id}});
}

bool PlaylistRepository::updatePlaylistCover(const QString &path, int playlist_id, int cover_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->update(db,
                            "playlists",
                            {{"cover_path", path}, {"cover_id", cover_id}},
                            {{"id", playlist_id}});
}

bool PlaylistRepository::deletePlaylist(int playlist_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->executeTransaction(db, [this, playlist_id](QSqlDatabase db){
        return m_dbHelper->remove(db, "playlists", {{"id", playlist_id}})
               &&
            m_dbHelper->remove(db, "playlist_songs", {{"playlist_id", playlist_id}});
    });
}

bool PlaylistRepository::updatePlaylistName(const QString &name, int playlist_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    return m_dbHelper->update(db,
                            "playlists",
                            {{"name", name}},
                            {{"id", playlist_id}});
}

QList<PlayListInfo> PlaylistRepository::queryPlaylists()
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db,
                                                             "playlists",
                                                             {"*"});

    QList<PlayListInfo> list;
    for(const auto &record : results){
        PlayListInfo info;
        info.id = record.value("id").toInt();
        info.name = record.value("name").toString();
        info.coverPath = record.value("cover_path").toString();
        info.cover_id = record.value("cover_id").toInt();

        list.append(info);
    }

    return list;
}

PlayListInfo PlaylistRepository::queryOneOfPlaylists(const QString &name)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QueryOptions opts;
    opts.conds = {{"name", name}};

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db,
                                                             "playlists",
                                                             {"id", "name", "cover_path"},
                                                             opts);

    if(results.isEmpty()){
        return {};
    }

    PlayListInfo info;

    QMap<QString, QVariant> record = results.first();

    info.id = record.value("id").toInt();
    info.name = record.value("name").toString();
    info.coverPath = record.value("cover_path").toString();

    return info;
}

QSet<int> PlaylistRepository::queryPlaylistId(int playlist_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QueryOptions opts;
    opts.conds = {{"playlist_id", playlist_id}};
    opts.orderBy =  {"sort_order DESC"};

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db,
                                                             "playlist_songs",
                                                             {"song_id"},
                                                             opts);

    QSet<int> list;

    for(const auto &record : results){
        list.insert(record.value("song_id").toInt());
    }

    return list;
}

int PlaylistRepository::songCountInPlaylist(int playlist_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QueryOptions opts;
    opts.conds = {{"playlist_id", playlist_id}};

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db,
                                                             "playlist_songs",
                                                             {"count(*)"},
                                                             opts);
    return results.size() > 0 ? results[0].value("count(*)").toInt() : -1;
}

QSet<int> PlaylistRepository::findPlaylistsBySong(int song_id)
{
    QSqlDatabase db = DbManager::getInstance().getConnection();

    QueryOptions opts;
    opts.conds = {{"song_id", song_id}};

    QList<QMap<QString, QVariant>> results = m_dbHelper->query(db,
                                                             "playlist_songs",
                                                             {"playlist_id"},
                                                             opts);

    QSet<int> list;

    for(const auto &record : results){
        list.insert(record.value("playlist_id").toInt());
    }

    return list;
}
