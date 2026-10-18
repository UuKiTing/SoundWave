#include "song_list_loader.h"
#include "dbmanager.h"
#include "song_list_model.h"
#include "song_info.h"
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtConcurrent>

SongListLoader::SongListLoader(QObject *parent)
    : QObject{parent}
{
    m_httpRequest = new HttpRequest(this);

    connect(m_httpRequest, &HttpRequest::networkErrored, this,
            [this](QNetworkReply::NetworkError error, const QString &errorString){
        qCWarning(appLog) << "远程歌曲列表加载失败:" << error << errorString;
        emit remoteSongsLoaded();
    });
}

void SongListLoader::loadLocalSongs(SongListModel *model)
{
    QFuture<QList<SongInfo>> future = QtConcurrent::run([this]{
        return m_songRepo.loadSongs();
    });

    auto *watcher = new QFutureWatcher<QList<SongInfo>>(this);

    connect(watcher, &QFutureWatcher<QList<SongInfo>>::finished, this, [this, watcher, model](){
        QList<SongInfo> songs = watcher->result();

        for(SongInfo &song : songs){
            model->addSong(song);
        }

        emit localSongsLoaded();

        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

void SongListLoader::loadRemoteSongs(SongListModel *model)
{
    m_httpRequest->get(Url::songsJsonUrl(), [this, model](QByteArray data){
        if(data.isEmpty()){
            qCWarning(appLog) << "远程歌曲列表返回为空";
            emit remoteSongsLoaded();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray array =  doc.array();

        QList<int> list = m_songRepo.queryCollectSongs();

        for(const auto &value : array){
            QJsonObject obj = value.toObject();

            SongInfo song;

            song.id = obj["id"].toString().toInt();
            song.source = SongSource::Remote;
            song.title = obj["title"].toString();
            song.artist = obj["artist"].toString();
            song.duration = obj["duration"].toString().toInt();
            song.durationString = toDurationString(song.duration);
            song.audioPath = Url::songAudioUrl() + obj["filePath"].toString();
            song.coverPath = Url::songImageUrl() + obj["coverPath"].toString();
            song.lyricsPath = Url::songLyricsUrl() + obj["lyricsPath"].toString();
            song.isFavorite = list.contains(song.id) ? true : false;

            model->addSong(song);
        }

        emit remoteSongsLoaded();
    });
}
