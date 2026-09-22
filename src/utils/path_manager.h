#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H

#include <QString>

namespace Paths {

/** @brief 数据目录 */
QString dataDir(); ///<

/** @brief 数据库文件路径 */
QString databasePath(); ///<

/** @brief 歌曲音频目录 */
QString songAudioDir();
QString songAudioDir(const QString& filePath);

/** @brief 歌曲图片目录 */
QString songImagesDir();
QString songImagesDir(const QString& filePath);

/** @brief 歌曲歌词目录 */
QString songLyricsDir(); ///<
QString songLyricsDir(const QString& filePath);

/** @brief 日志目录 */
QString logsDir(); ///<
QString logsDir(const QString& filePath);

bool ensureDirectories();


}
#endif // PATH_MANAGER_H
