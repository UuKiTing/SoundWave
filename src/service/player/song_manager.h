#ifndef SONG_MANAGER_H
#define SONG_MANAGER_H

#include "dbmanager.h"
#include "collect_proxy_model.h"
#include "search_proxy_model.h"
#include "playlist_proxy_model.h"
#include "local_proxy_model.h"
#include "remote_proxy_model.h"
#include "song_list_model.h"
#include "song_playback_state.h"
#include "song_list_loader.h"
#include "song_repository.h"
#include <QObject>
#include <QStandardItemModel>
#include <QList>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QAbstractListModel>

class SongManager : public QObject
{
    Q_OBJECT

public:
    explicit SongManager(QObject *parent = nullptr);

    // 设置模型中某行的数据项的收藏状态
    bool setPlayingStatus(const QModelIndex &index);

    // 歌曲收藏或者取消收藏
    bool setFavorite(const QModelIndex &index, bool isCollect);

    // 设置模型中某行的数据项的进度值
    bool setProgress(const QModelIndex &index, int progress);

    // 设置模型中某行的数据项为无效状态
    bool setInvalid(const QModelIndex &index);

    // 添加歌曲到模型中
    void appendSong(const SongInfo &song);

    // 返回当前播放歌曲的行号
    int currentRow();

    // 返回当前播放模式
    PlayMode playMode();

    // 设置播放模式
    void setPlayMode(PlayMode mode);

    // 设置当前播放列表的行数
    void setListRows(int rows);

    void setListRows(const QAbstractItemModel *model);

    // 设置代理模型id
    void setProxyId(ProxyId id);

    // 获取代理模型id
    ProxyId currentProxyId();

    // 设置当前哪个歌单正在播放
    void setCurrentPlaylist(int playlist_id);

    // 获取播放的歌单id
    int currentPlaylist();

    // 返回代理模型
    LocalProxyModel* localModel();
    CollectProxyModel* collectModel();
    SearchProxyModel* searchModel();
    PlayListProxyModel* playlistModel();
    RemoteProxyModel* remoteModel();

    // 设置当前播放页面类型
    void setPlayPage(MainPage page);

    // 获取当前播放页面类型
    MainPage playingPage();

    // 根据代理模型Id返回代理模型所在的主页面类型
    MainPage pageOfProxyId(ProxyId id);

    // 设置当前播放歌曲的模型索引
    bool setCurrentIndex(const QModelIndex &index);

    // 获取当前播放歌曲的模型索引
    QModelIndex currentIndex();

    // 设置上/下一首歌曲的模型索引
    QModelIndex setNextIndex(bool isNext);

    // 根据代理模型 id, 返回代理模型
    QSortFilterProxyModel* proxyModel(ProxyId id);

    // 返回源模型索引
    static QModelIndex mapToSource(const QModelIndex &index);

signals:
    // 播放模式更改信号
    void playModeChanged(PlayMode mode);

    // 歌曲加载完毕信号
    void songsLoaded();

    // 当前播放页更改信号
    void playPageChanged(MainPage page);

    // 当前行号更改信号
    void currentRowChanged(int row);

    // 当前歌单更改信号
    void currentPlaylistChanged(int playlist_id);

public slots:
    // 更改播放模式
    void changePlayMode();

private:
    // 加载本地歌曲和云端歌曲数据
    void loadSongs();

    SongListModel *m_songlistModel;
    CollectProxyModel *m_collectModel{};
    SearchProxyModel *m_searchModel{};
    PlayListProxyModel *m_playlistModel{};
    LocalProxyModel *m_localModel{};
    RemoteProxyModel *m_remoteModel{};

    SongPlaybackState *m_playbackState{};

    SongListLoader *m_listLoader;

    SongRepository m_songRepo;
};

#endif // SONG_MANAGER_H
