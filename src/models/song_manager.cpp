#include "song_manager.h"
#include "image_loader_global.h"
#include "path_manager.h"
#include "model_roles.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFuture>

SongManager::SongManager(QObject *parent)
    : QObject{parent}
{
    m_songlistModel = new SongListModel(this);

    m_localModel = new LocalProxyModel(this);
    m_localModel->setSourceModel(m_songlistModel);
    m_localModel->setProperty("proxyId", QVariant::fromValue(ProxyId::Local));

    m_collectModel = new CollectProxyModel(this);
    m_collectModel->setSourceModel(m_songlistModel);
    m_collectModel->setProperty("proxyId", QVariant::fromValue(ProxyId::Collect));

    m_searchModel = new SearchProxyModel(this);
    m_searchModel->setSourceModel(m_songlistModel);
    m_searchModel->setProperty("proxyId", QVariant::fromValue(ProxyId::Search));

    m_playlistModel = new PlayListProxyModel(this);
    m_playlistModel->setSourceModel(m_songlistModel);
    m_playlistModel->setProperty("proxyId", QVariant::fromValue(ProxyId::Playlist));

    m_remoteModel = new RemoteProxyModel(this);
    m_remoteModel->setSourceModel(m_songlistModel);
    m_remoteModel->setProperty("proxyId", QVariant::fromValue(ProxyId::Remote));

    m_playbackState = new SongPlaybackState(this);

    m_listLoader = new SongListLoader(this);

    loadSongs();
}


void SongManager::loadSongs()
{
    m_listLoader->loadLocalSongs(m_songlistModel);
    m_listLoader->loadRemoteSongs(m_songlistModel);

    connect(m_listLoader, &SongListLoader::localSongsLoaded, this, [this](){
        emit songsLoaded();
    });

    connect(m_listLoader, &SongListLoader::remoteSongsLoaded, this, [this](){
        emit songsLoaded();
    });
}

bool SongManager::setPlayingStatus(const QModelIndex &index)
{
    QModelIndex sourceIndex = this->mapToSource(index);
    if(sourceIndex.isValid()){
        return m_songlistModel->setPlayingStatus(sourceIndex.row());
    }
    return false;
}

bool SongManager::setFavorite(const QModelIndex &index, bool isCollect)
{
    int song_id = index.data(Roles::Id).toInt();

    bool dbOk = false;

    if(isCollect) dbOk = DbManager::getInstance().collectSong(song_id);
    else  dbOk = DbManager::getInstance().disCollectSong(song_id);

    if(!dbOk){
        return false;
    }

    QPair<int, int> rows = m_songlistModel->value(song_id);

    return m_songlistModel->setFavorite(rows.first, isCollect)
           && (rows.second >= 0 ? m_songlistModel->setFavorite(rows.second, isCollect) : true);
}

bool SongManager::setProgress(const QModelIndex &index, int progress)
{
    QModelIndex sourceIndex = this->mapToSource(index);
    if(sourceIndex.isValid()){
        return m_songlistModel->setProgress(sourceIndex.row(), progress);
    }
    return false;
}

bool SongManager::setInvalid(const QModelIndex &index)
{
    QModelIndex sourceIndex = this->mapToSource(index);
    if(sourceIndex.isValid()){
        return m_songlistModel->setInvalid(sourceIndex.row());
    }
    return false;
}

void SongManager::appendSong(const SongInfo &song)
{
    m_songlistModel->addSong(song);
}

int SongManager::currentRow()
{
    return m_playbackState->currentRow();
}

void SongManager::setPlayMode(PlayMode mode)
{
    m_playbackState->setPlayMode(mode);
    emit playModeChanged(mode);
}

void SongManager::setListRows(int rows)
{
    m_playbackState->setListRows(rows);
}

void SongManager::setProxyId(ProxyId id)
{
    m_playbackState->setProxyId(id);
}

ProxyId SongManager::proxyId()
{
    return m_playbackState->proxyId();
}

void SongManager::setPlaylistPlayingNumber(int number)
{
    m_playbackState->setPlaylistPlayingNumber(number);
}

int SongManager::playlistPlayingNumber()
{
    return m_playbackState->playlistPlayingNumber();
}

void SongManager::changePlayMode()
{
    m_playbackState->changePlayMode();
    emit playModeChanged(m_playbackState->playMode());
}

LocalProxyModel *SongManager::localModel()
{
    return m_localModel;
}

CollectProxyModel *SongManager::collectModel()
{
    return m_collectModel;
}

SearchProxyModel *SongManager::searchModel()
{
    return m_searchModel;
}

PlayListProxyModel *SongManager::playlistModel()
{
    return m_playlistModel;
}

RemoteProxyModel *SongManager::remoteModel()
{
    return m_remoteModel;
}

PlayMode SongManager::playMode()
{
    return m_playbackState->playMode();
}

QSortFilterProxyModel *SongManager::proxyModel(ProxyId id)
{
    switch (id) {
    case ProxyId::Local:    return m_localModel;
    case ProxyId::Remote:  return m_remoteModel;
    case ProxyId::Collect: return m_collectModel;
    case ProxyId::Playlist:   return m_playlistModel;
    case ProxyId::Search:    return m_searchModel;
    }

    return nullptr;
}

void SongManager::setPlayPage(MainPage page)
{
    m_playbackState->setCurrentPage(page);
}

MainPage SongManager::playingPage()
{
    return m_playbackState->currentPage();
}

MainPage SongManager::pageOfProxyId(ProxyId id)
{
    switch (id) {
    case ProxyId::Local:    return MainPage::Local;
    case ProxyId::Remote:  return MainPage::Remote;
    case ProxyId::Collect: return MainPage::Collect;
    case ProxyId::Playlist:   return MainPage::PlayList;
    default: return MainPage::Local;
    }
}

QModelIndex SongManager::currentIndex()
{
    int row = this->currentRow();
    ProxyId proxyId = this->proxyId();
    QSortFilterProxyModel *proxyModel = this->proxyModel(proxyId);

    if(!proxyModel){
        return QModelIndex();
    }

    return proxyModel->index(row, 0);
}

bool SongManager::setCurrentIndex(const QModelIndex &index)
{
    if(!index.isValid()) return false;

    m_playbackState->setCurrentRow(index.row());

    return true;
}

QModelIndex SongManager::setNextIndex(bool isNext)
{
    m_playbackState->setNextRow(isNext);
    return this->currentIndex();
}

QModelIndex SongManager::mapToSource(const QModelIndex &index)
{
    if(!index.isValid()) return QModelIndex();

    const QAbstractProxyModel  *proxyModel = qobject_cast<const QAbstractProxyModel *>(index.model());

    QModelIndex sourceIndex;
    if(!proxyModel) sourceIndex = index;
    else sourceIndex = proxyModel->mapToSource(index);

    return sourceIndex;
}
