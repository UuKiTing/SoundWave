#include "song_list_loader.h"
#include "dbmanager.h"
#include "song_list_model.h"
#include <QDir>
#include <QCoreApplication>

SongListLoader::SongListLoader(QObject *parent)
    : QObject{parent}
{}

void SongListLoader::loadSongs(SongListModel *model)
{
    QDir dir = QDir(QCoreApplication::applicationDirPath());

    for(SongInfo &song : DbManager::getInstance().loadSongs()){

        QString filePath = dir.filePath(song.filePath);
        QString coverPath = dir.filePath(song.coverPath);

        song.filePath = filePath;
        song.coverPath = coverPath;

        model->addSong(song);
    }
}
