#include "path_manager.h"
#include <QDir>
#include <QCoreApplication>

namespace Paths {

QString dataDir()
{
    return "data";
}

QString databasePath()
{
    return QDir(dataDir()).filePath("music.db");
}

QString songAudioDir()
{
    return QDir(dataDir()).filePath("songAudio");
}

QString songImagesDir()
{
    return QDir(dataDir()).filePath("songImage");
}

QString songLyricsDir()
{
    return QDir(dataDir()).filePath("songLyrics");
}

QString logsDir()
{
    return QDir(dataDir()).filePath("logs");
}

QString logsDir(const QString &filePath)
{
    return QDir(logsDir()).filePath(filePath);
}

QString songAudioDir(const QString &filePath)
{
    return QDir(songAudioDir()).filePath(filePath);
}

QString songImagesDir(const QString &filePath)
{
    return QDir(songImagesDir()).filePath(filePath);
}

QString songLyricsDir(const QString &filePath)
{
    return QDir(songLyricsDir()).filePath(filePath);
}

bool ensureDirectories()
{
    QDir dir;
    return dir.mkpath(dataDir())
           && dir.mkpath(songAudioDir())
           && dir.mkpath(songImagesDir())
           && dir.mkpath(songLyricsDir())
           && dir.mkpath(logsDir());
}


}
