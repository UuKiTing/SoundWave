#include "song_list_loader.h"
#include "dbmanager.h"
#include "song_list_model.h"
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

SongListLoader::SongListLoader(QObject *parent)
    : QObject{parent}
{
    httpClient = new HttpClient(this);
}

void SongListLoader::loadSongs(SongListModel *model)
{
    for(SongInfo &song : DbManager::getInstance().loadSongs()){
        model->addSong(song);
    }
}

void SongListLoader::loadRemoteSongs(SongListModel *model)
{
    httpClient->get(SongsJsonUrl, [this, model](QByteArray data){
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray array =  doc.array();

        QList<int> list = DbManager::getInstance().queryColletSongs();

        for(auto value : array){
            QJsonObject obj = value.toObject();

            SongInfo song;

            song.id = obj["id"].toInt();
            song.source = SongSource::Remote;
            song.title = obj["title"].toString();
            song.artist = obj["artist"].toString();
            song.duration = obj["duration"].toString().toInt();
            song.durationString = toDurationString(song.duration);
            song.filePath = SongAudioUrl + obj["filePath"].toString();
            song.coverPath = SongImageUrl + obj["coverPath"].toString();
            song.lyricsPath = SongLyricsUrl + obj["lyricsPath"].toString();
            song.isFavo = list.contains(song.id) ? true : false;
            song.isPlaying = false;

            model->addSong(song);
        }
    });

}
