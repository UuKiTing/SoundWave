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

    /**
     * @brief 将代理模型索引转为源模型行号后，转发至SongListModel::setPlayingStatus
     * @param index 代理模型索引
     * @return bool 设置成功返回 true，索引无效返回 false
     */
    bool setPlayingStatus(const QModelIndex &index);

    /**
     * @brief 执行数据库收藏或者取消收藏操作。
     *
     * 如果数据库操作成功，将代理模型索引转为源模型行号后，
     * 转发至SongListModel::ssetFavorite，
     * 如果数据库操作失败则返回false
     *
     * @param index 模型索引
     * @param isCollect 是否收藏
     * @return bool 操作成功返回 true，操作失败或者索引无效返回 false
     */
    bool setFavorite(const QModelIndex &index, bool isCollect);

    /**
     * @brief 将代理模型索引转为源模型行号后，转发至SongListModel::setProgress
     * @param index 代理模型索引
     * @param progress 进度值
     * @return bool 设置成功返回 true，索引无效返回 false
     */
    bool setProgress(const QModelIndex &index, int progress);

    /**
     * @brief 将代理模型索引转为源模型行号后，转发至SongListModel::setValid
     * @param index 代理模型索引
     * @param isValid ture=有效, false=无效
     * @return bool 设置成功返回 true，索引无效返回 false
     */
    bool setInvalid(const QModelIndex &index);

    /** @brief 添加歌曲到模型中 */
    void appendSong(const SongInfo &song);


    /**
     * @brief 直接转发至SongPlaybackSate::setNextRow
     * @return int 歌曲行号
     */
    int currentRow();

    /**
     * @brief 直接转发至SongPlaybackSate::playMode
     * @return PlayMode 播放模式
     */
    PlayMode playMode();

    /**
     * @brief 直接转发至SongPlaybackSate::setPlayMode
     *        自动发出playModeChanged信号
     * @param mode 播放模式
     */
    void setPlayMode(PlayMode mode);

    /**
     * @brief 直接转发至SongPlaybackSate::setListRows
     * @param rows 行数
     */
    void setListRows(int rows); // 设置播放列表的索引行号

    /**
     * @brief 直接转发至SongPlaybackSate::setProxyId
     * @param id 代理模型id
     */
    void setProxyId(ProxyId id);

    /**
     * @brief 直接转发至SongPlaybackSate::proxyId
     * @return ProxyId 代理模型id
     */
    ProxyId proxyId();

    /**
     * @brief 直接转发至SongPlaybackSate::setPlaylistPlayingNumber
     * @param number 歌单序号
     */
    void setPlaylistPlayingNumber(int number);

    /**
     * @brief 直接转发至SongPlaybackSate::playlistPlayingNumber
     * @return int 歌单序号
     */
    int playlistPlayingNumber();

    /** @brief 返回代理模型 */
    LocalProxyModel* localModel();
    CollectProxyModel* collectModel();
    SearchProxyModel* searchModel();
    PlayListProxyModel* playlistModel();
    RemoteProxyModel* remoteModel();

    /**
     * @brief 直接转发至SongPlaybackSate::setPlayPage
     * @param page 主页面类型
     */
    void setPlayPage(MainPage page);

    /**
     * @brief 直接转发至SongPlaybackSate::playingPage
     * @return MainPage 主页面类型
     */
    MainPage playingPage();

    /**
     * @brief 根据代理模型Id返回代理模型所在的主页面类型
     * @param id 代理模型id
     * @return MainPage 主页面类型
     */
    MainPage pageOfProxyId(ProxyId id);

    /**
     * @brief 返回当前播放歌曲的模型索引
     * @return QModelIndex 模型索引
     */
    QModelIndex currentIndex();

    /**
     * @brief 设置当前播放歌曲的模型索引
     * @param index 模型索引
     * @return bool 设置成功返回 true，失败则返回 false
     */
    bool setCurrentIndex(const QModelIndex &index);

    /**
     * @brief 设置上/下一首歌曲的模型索引
     *
     * 内部会自动调用SongPlaybackSate::setCurrentRow函数
     *
     * @param isNext true=下一首, false=上一首
     * @return QModelIndex 模型索引
     */
    QModelIndex setNextIndex(bool isNext);

    /**
     * @brief 根据地理模型id返回代理模型
     * @param id 代理模型id
     * @return QSortFilterProxyModel* 代理模型
     */
    QSortFilterProxyModel* proxyModel(ProxyId id);

    /**
     * @brief 返回源模型索引
     * @param index 模型索引
     * @return QModelIndex 源模型索引
     */
    static QModelIndex mapToSource(const QModelIndex &index);

signals:
    /**
     * @brief 播放模式更改信号
     * @param mode 播放模式
     */
    void playModeChanged(PlayMode mode);

    /**
     * @brief 歌曲加载完毕信号
     * @param isDownload true=已下载, false=未下载
     */
    void songsLoaded();

public slots:
    /** @brief 播放模式更改槽函数，更改完成后自动触发playModeChanged信号 */
    void changePlayMode();

private:
    /**  @brief 加载本地歌曲和云端歌曲数据 */
    void loadSongs();

    SongListModel *m_songlistModel;
    CollectProxyModel *m_collectModel{};
    SearchProxyModel *m_searchModel{};
    PlayListProxyModel *m_playlistModel{};
    LocalProxyModel *m_localModel{};
    RemoteProxyModel *m_remoteModel{};

    SongPlaybackState *m_playbackState{};

    SongListLoader *m_listLoader;
};

#endif // SONG_MANAGER_H
