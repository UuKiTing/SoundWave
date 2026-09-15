#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QPixmap>
#include <QModelIndex>
#include <QMetaType>


enum class SongSource{
    Local = 0,  // 内置/本地歌曲
    Remote = 1,  // 网络在线歌曲
};


struct SongInfo{
    qlonglong id = -1;
    qlonglong remoteId = -1;
    SongSource source = SongSource::Local;
    QString title; // 歌名
    QString artist; // 歌手
    int duration = 0; // 时长/秒
    QString durationString;
    QString filePath; // 文件路径/网络音频流http地址
    QString coverPath; // 封面路径
    QString lyricsPath; // 歌词路径
    bool isFavo; // 是否收藏
    bool isPlaying; // 是否正在播放
};

enum Roles{
    Id = Qt::UserRole + 1,
    RemoteId,
    Source,
    Title,
    Artist,
    Duration,
    DurationString,
    FilePath,
    CoverPath,
    LyricsPath,
    IsPlaying,
    IsFavorite,

};


struct PlayListInfo{
    qlonglong id = -1;
    QString name; // 歌单名称
    int songCount = 0; // 歌曲数量
    QString cover; // 封面路径
};

enum PlayMode{
    Loop,
    Random,
    Single,
    Count
};

enum Page {
    Local = 0,
    Collect,
    PlayList,
    NetWork,
    Setting
};

class SearchPage {
public:
    enum StackedWidgetIndex {
        Main = 0,
        Search
    };
};

enum class ProxyId : int {
    Local = 0,
    NetWork = 1,
    Collect = 2,
    SongList = 3,
    Search = 4
};

QString toDurationString(int duration); // 返回歌曲时长的HH:mm格式

QPixmap roundPixmap(const QPixmap &source, const QSize &size, int radius);

QPixmap roundPixmap(const QString &path, const QSize &size, int radius);

const QPixmap& defaultCover();

#endif // GLOBAL_H

