#ifndef SONG_INFO_H
#define SONG_INFO_H

#include <QString>

/**
 * @brief 歌曲来源
 */
enum class SongSource: int{
    Local = 0, //< 本地下载
    Remote = 1, //< 云端
};

/**
 * @brief 歌曲元数据
 */
struct SongInfo{
    int id = -1; ///< 歌曲 id
    SongSource source = SongSource::Local; ///< 歌曲来源
    QString title; ///< 歌曲标题
    QString artist; ///< 歌曲作者
    int duration = 0; ///< 时长/秒
    QString durationString = ""; ///< 时长格式化字符串: MM:SS
    QString audioPath = ""; ///< 歌曲音频文件路径
    QString coverPath = ""; ///< 歌曲封面文件路径
    QString lyricsPath = ""; ///< 歌曲歌词文件路径
    bool isFavorite = false; ///< 是否收藏
    bool isPlaying = false; ///< 是否正在播放
    int progressValue = -1; ///< 下载进度值
    bool isValid = true;
};

/**
 * @brief 返回时长的格式化字符串 MM:SS
 * @param duration 时长/s
 * @return QString
 */
inline QString toDurationString(int duration){
    QString minutes = QString("%1").arg(duration / 60, 2, 10, QChar('0'));
    QString seconds = QString("%1").arg(duration % 60, 2, 10, QChar('0'));
    return minutes + ":" + seconds;
}

#endif // SONG_INFO_H
