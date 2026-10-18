#ifndef SONG_LIST_LOADER_H
#define SONG_LIST_LOADER_H


#include "http_request.h"
#include "url_config.h"
#include "logging.h"
#include "song_repository.h"
#include <QObject>
#include <QAbstractListModel>


class SongListModel;
struct SongInfo;

// 歌曲数据加载器
class SongListLoader : public QObject
{
    Q_OBJECT
public:
    explicit SongListLoader(QObject *parent = nullptr);

    // 加载本地歌曲数据到模型中
    void loadLocalSongs(SongListModel *model);

    // 加载云端歌曲数据到模型中
    void loadRemoteSongs(SongListModel *model);

signals:
    void localSongsLoaded();
    void remoteSongsLoaded();

private:
    HttpRequest *m_httpRequest;
    SongRepository m_songRepo;
};

#endif // SONG_LIST_LOADER_H
