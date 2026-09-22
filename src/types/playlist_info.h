#ifndef PLAYLIST_INFO_H
#define PLAYLIST_INFO_H

#include <QString>

/**
 * @brief 定义歌单元数据
 */
struct PlayListInfo{
    qlonglong id = -1; ///< 歌单id
    QString name = ""; ///< 歌单名称
    QString coverPath = ""; ///< 歌单封面路径
};

#endif // PLAYLIST_INFO_H
