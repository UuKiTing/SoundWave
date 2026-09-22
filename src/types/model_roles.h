#ifndef MODEL_ROLES_H
#define MODEL_ROLES_H

#include <QObject>


/**
 * @brief 自定义模型的角色项
 */
enum Roles{
    Id = Qt::UserRole + 1, ///< 歌曲Id
    Source, ///< 数据源
    Title, ///< 歌曲标题
    Artist, ///< 歌曲作者
    Duration, ///< 歌曲时长
    AudioPath, ///< 歌曲音频文件路径
    CoverPath, ///< 歌曲封面文件路径
    LyricsPath, ///< 歌曲歌词文件路径
    IsPlaying, ///< 是否正在播放
    IsFavorite, ///< 是否收藏
    ProgressValue, ///< 下载进度值
    IsValid, ///< 是否有效
};


#endif // MODEL_ROLES_H
