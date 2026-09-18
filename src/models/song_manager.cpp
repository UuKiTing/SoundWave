#include "song_manager.h"
#include "image_loader_global.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QRandomGenerator>
#include <taglib/tag.h>
#include <taglib/fileref.h>
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
    m_playlistModel->setProperty("proxyId", QVariant::fromValue(ProxyId::SongList));

    m_remoteModel = new RemoteProxyModel(this);
    m_remoteModel->setSourceModel(m_songlistModel);
    m_remoteModel->setProperty("proxyId", QVariant::fromValue(ProxyId::NetWork));

    m_playbackState = new SongPlayBackSate(this);

    m_listLoader = new SongListLoader(this);

    loadSongs();    
}



void SongManager::loadSongs()
{
    m_listLoader->loadSongs(m_songlistModel);

    m_listLoader->loadRemoteSongs(m_songlistModel);
}


void SongManager::generateData()
{
    int id = 1;
    QString currentPath = QCoreApplication::applicationDirPath();
    QDir dir(QDir(currentPath).filePath("songs"));


    for(const QFileInfo &info : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)){
        QJsonObject obj = parseMusic(info.filePath());

        SongInfo song;
        song.id = id++;
        song.title = obj["title"].toString();
        song.artist = obj["artist"].toString();
        song.duration = obj["duration"].toInt();
        song.filePath = "songs/" + info.fileName();
        song.coverPath = "songImage/" + info.baseName() + ".png";
        song.lyricsPath = "songLyrics/" + info.baseName() + ".lrc";

        DbManager::getInstance().appendMusicData(song);
    }
}


QJsonObject SongManager::parseMusic(const QString &filePath)
{
    // 创建 FileRef 对象（自动识别格式）
    TagLib::FileRef file(filePath.toStdWString().c_str());

    QJsonObject obj;

    // 检查文件是否有效、是否有标签信息
    if (!file.isNull() && file.tag()) {
        TagLib::Tag *tag = file.tag();

        obj["title"] = QString::fromStdWString(tag->title().toWString());
        obj["artist"] = QString::fromStdWString(tag->artist().toWString());
        obj["duration"] = file.audioProperties()->lengthInSeconds();
    }

    return obj;
}

void SongManager::setCurrentIndex(const QModelIndex &index)
{
    m_playbackState->setCurrentIndex(index);
}

bool SongManager::setPlayingStatus(const QModelIndex &index)
{
    QModelIndex sourceIndex = this->mapToSource(index);
    if(sourceIndex.isValid()){
        m_songlistModel->setPlayingStatus(sourceIndex.row());
    }
}

bool SongManager::setFavorite(const QModelIndex &index, bool isCollect)
{
    QModelIndex sourceIndex = this->mapToSource(index);
    if(sourceIndex.isValid()){
        m_songlistModel->setFavorite(sourceIndex.row(), isCollect);
    }
}

QModelIndex SongManager::setNextIndex(bool isNext)
{
    return m_playbackState->setNextIndex(isNext);
}

void SongManager::setMode(PlayMode mode)
{
    m_playbackState->setMode(mode);
    emit modeChanged(mode);
}

void SongManager::setListRows(int rows)
{
    m_playbackState->setListRows(rows);
}

void SongManager::setPlayPage(Page page)
{
    m_playbackState->setCurrentPage(page);
}

void SongManager::setPalylistLastNumber(int number)
{
    m_playbackState->setPlaylistLastNumber(number);
}

void SongManager::changePlayMode()
{
    m_playbackState->changeMode();
    emit modeChanged(m_playbackState->mode());
}

QAbstractItemModel *SongManager::songlistModel()
{
    return m_songlistModel;
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

SongPlayBackSate *SongManager::playbackState()
{
    return m_playbackState;
}


QModelIndex SongManager::currentIndex()
{
    return m_playbackState->currentIndex();
}

QModelIndex SongManager::sourceIndex(int row)
{
    return m_songlistModel->index(row, 0);
}

int SongManager::currentRow()
{
    return m_playbackState->currentRow();
}

PlayMode SongManager::mode()
{
    return m_playbackState->mode();
}

QSortFilterProxyModel *SongManager::proxyModel(ProxyId id)
{
    switch (id) {
    case ProxyId::Local:    return m_localModel;
    case ProxyId::NetWork:  return m_remoteModel;
    case ProxyId::Collect: return m_collectModel;
    case ProxyId::SongList:   return m_playlistModel;
    case ProxyId::Search:    return m_searchModel;
    }

    return nullptr;
}

Page SongManager::playPage()
{
    return m_playbackState->currentPage();
}

Page SongManager::pageOfProxy(ProxyId id)
{
    switch (id) {
    case ProxyId::Local:    return Page::Local;
    case ProxyId::NetWork:  return Page::NetWork;
    case ProxyId::Collect: return Page::Collect;
    case ProxyId::SongList:   return Page::PlayList;
    default: return Page::Local;
    }
}

int SongManager::playlistLastNumber()
{
    return m_playbackState->playlistLastNumber();
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
