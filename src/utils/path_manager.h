#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <QString>

namespace Paths {

// 数据目录
QString dataDir();

// 数据库文件路径
QString databasePath();

// 歌曲音频目录
QString songAudioDir();
QString songAudioDir(const QString& filePath);

// 歌曲图片目录
QString songImagesDir();
QString songImagesDir(const QString& filePath);

// 歌曲歌词目录
QString songLyricsDir();
QString songLyricsDir(const QString& filePath);

// 日志目录
QString logsDir();
QString logsDir(const QString& filePath);

QString configFilePath();

bool ensureDirectories();


}
#endif // PATH_MANAGER_H
