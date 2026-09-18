#ifndef SONG_MANAGER_H
#define SONG_MANAGER_H

#include "dbmanager.h"
#include "collect_proxy_model.h"
#include "search_proxy_model.h"
#include "song_list_proxy_model.h"
#include "local_proxy_model.h"
#include "remote_proxy_model.h"
#include "song_list_model.h"
#include "song_playback_sate.h"
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

    bool setPlayingStatus(const QModelIndex &index);
    bool setFavorite(const QModelIndex &index, bool isCollect);

    void setCurrentIndex(const QModelIndex &index); // 设置model当前的行号
    QModelIndex setNextIndex(bool isNext); // 设置下一首歌曲的行号
    void setMode(PlayMode mode); // 设置播放模式
    void setListRows(int rows); // 设置播放列表的索引行号
    void setPlayPage(Page page);
    void setPalylistLastNumber(int number);

    QAbstractItemModel* songlistModel();
    LocalProxyModel* localModel(); // 返回本地代理模型
    CollectProxyModel* collectModel(); // 返回收藏代理model
    SearchProxyModel* searchModel(); // 返回搜索代理model
    PlayListProxyModel* playlistModel(); // 返回歌单代理model
    RemoteProxyModel* remoteModel();
    SongPlayBackSate* playbackState();

    QModelIndex currentIndex(); // 返回当前行的代理index
    QModelIndex sourceIndex(int row);
    PlayMode mode();  // 返回播放模式
    int currentRow();
    Page playPage();
    Page pageOfProxy(ProxyId id);
    int playlistLastNumber();

    QSortFilterProxyModel* proxyModel(ProxyId id); // 根据id返回具体代理模型
    static QModelIndex mapToSource(const QModelIndex &index); // 返回源模型的index

signals:
    void modeChanged(PlayMode mode); // 播放模式更改信号

public slots:
    void changePlayMode();

private:
    void loadSongs(); // 加载歌曲

    void generateData(); // 生成音乐数据

    QJsonObject parseMusic(const QString &filePath); // 解析音乐文件

    SongListModel *m_songlistModel;
    CollectProxyModel *m_collectModel{};
    SearchProxyModel *m_searchModel{};
    PlayListProxyModel *m_playlistModel{};
    LocalProxyModel *m_localModel{};
    RemoteProxyModel *m_remoteModel{};

    SongPlayBackSate *m_playbackState{};

    SongListLoader *m_listLoader;
};

#endif // SONG_MANAGER_H
