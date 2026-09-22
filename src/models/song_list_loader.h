#ifndef SONG_LIST_LOADER_H
#define SONG_LIST_LOADER_H


#include "http_request.h"
#include "url_config.h"
#include <QObject>
#include <QAbstractListModel>


class SongListModel;
struct SongInfo;

/**
 * @brief 歌曲数据加载器
 */
class SongListLoader : public QObject
{
    Q_OBJECT
public:
    explicit SongListLoader(QObject *parent = nullptr);

    /**
     * @brief 加载本地歌曲数据到模型中
     * @param model 歌曲列表模型
     */
    void loadLocalSongs(SongListModel *model);

    /**
     * @brief 加载云端歌曲数据到模型中
     * @param model 歌曲列表模型
     */
    void loadRemoteSongs(SongListModel *model);
signals:
    void localSongsLoaded();
    void remoteSongsLoaded();

private:
    HttpRequest *m_httpRequest; ///< http请求对象
};

#endif // SONG_LIST_LOADER_H
