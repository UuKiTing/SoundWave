#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "song_info.h"
#include "playlist_info.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QModelIndex>
#include <QMutex>

/**
 * @brief 数据库管理
 * @note 单例模式
 */
class DbManager
{
public:
    ///< 禁用拷贝构造、赋值
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    /**
     * @brief 返回DbManager单例对象
     */
    static DbManager& getInstance();

    /**
     * @brief isValid
     * @return bool
     */
    bool isValid();

    /**
     * @brief 从数据库中加载本地歌曲数据
     * @return QList<SongInfo> 歌曲元数据列表
     */
    QList<SongInfo> loadSongs();

    /**
     * @brief 添加新的歌曲到数据库中
     * @param info 新歌曲的元信息
     * @return bool 添加成功返回 true，失败则回 false
     */
    bool appendSong(const SongInfo &info);


    /**
     * @brief 删除歌曲
     * @param info 新歌曲的元信息
     * @return bool 添加成功返回 true，失败则回 false
     */
    bool removeSong(int song_id);

    /**
     * @brief 收藏歌曲
     * @param song_id 收藏的歌曲id
     * @return bool 收藏成功返回 true，失败则回 false
     */
    bool collectSong(int song_id); // 收藏歌曲

    /**
     * @brief 取消收藏歌曲
     * @param song_id 取消收藏的歌曲id
     * @return bool 取消收藏成功返回 true，失败则回 false
     */
    bool disCollectSong(int song_id);

    /**
     * @brief 查询收藏的歌曲有哪些
     * @return QList<int> 歌曲id列表。
     */
    QList<int> queryCollectSongs();

    /**
     * @brief 创建歌单
     * @param name 歌单名称
     * @return PlayListInfo 歌单元数据
     */
    PlayListInfo createPlaylist(const QString &name);

    /**
     * @brief 插入歌曲到歌单中
     * @param palylist_id 歌单id
     * @param song_id 歌曲id
     * @return  bool 插入成功返回 true，失败则回 false
     */
    bool insertSongToPlaylist(int palylist_id, int song_id);

    /**
     * @brief 删除歌单中的歌曲
     * @param playlist_id 歌单id
     * @param song_id 歌曲id
     * @return  bool 删除成功返回 true，失败则回 false
     */
    bool removeSongToPlaylist(int playlist_id, int song_id);

    /**
     * @brief 更新歌单封面图片
     * @param path 图片路径
     * @param playlist_id 歌单id
     * @return bool 更新成功返回 true，失败则回 false
     */
    bool updatePlaylistCover(const QString &path, int playlist_id);

    /**
     * @brief 删除歌单
     * @param playlist_id 歌单id
     * @return bool 删除成功返回 true，失败则返回 false
     */
    bool deletePlaylist(int playlist_id); // 删除歌单

    /**
     * @brief 更新歌单名称
     * @param name 歌单名称
     * @param playlist_id 歌单id
     * @return bool 更新成功返回 true，失败则回 false
     */
    bool updatePlaylistName(const QString &name, int playlist_id);

    /**
     * @brief 查询所有歌单
     * @return QList<PlayListInfo> 歌单元数据列表。
     */
    QList<PlayListInfo> queryPlaylists();

    /**
     * @brief 查询某一歌单
     * @param name 歌单名称
     * @return PlayListInfo 歌单元数据
     */
    PlayListInfo  queryOneOfPlaylists(const QString &name);

    /**
     * @brief 查询某歌单下的所以歌曲id
     * @param playlist_id 歌曲id
     * @return QSet<int> 歌曲id集合
     */
    QSet<int> queryPlaylistId(int playlist_id);

    /**
     * @brief 查询某歌单在的歌曲数量
     * @param playlist_id 歌曲id
     * @return int 歌曲数量
     */
    int songCountInPlaylist(int playlist_id);

    /**
     * @brief 查找某歌曲所在的歌单
     * @param song_id 歌曲id
     * @return QSet<int> 歌单id集合
     */
    QSet<int> findPlaylistsBySong(int song_id);

private:
    /**
     * @brief 执行事务
     *
     * 自动管理事务的生命周期：开始事务 -> 执行回调 -> 成功则提交，失败则回滚
     *
     * @param function 事务回调函数，返回 true 表示提交，返回 false 表示回滚
     * @return bool 事务提交成功返回 true，否则返回 false
     */
    template<typename Function>
    bool executeTransaction(Function function);

    /**
     * @brief 创建歌曲表
     * @return 创建成功返回 true，失败则回 false
     */
    bool createSongTable();

    /**
     * @brief 创建收藏表
     * @return 创建成功返回 true，失败则回 false
     */
    bool createCollectionTable();

    /**
     * @brief 创建歌单表
     * @return 创建成功返回 true，失败则回 false
     */
    bool createPlaylistsTable();

    /**
     * @brief 创建歌单歌曲表
     * @return 创建成功返回 true，失败则回 false
     */
    bool createPlaylistSongsTable();

    ///< 私有构造和析构函数
    explicit DbManager();
    ~DbManager();

    QSqlDatabase m_db; ///< 数据库连接对象

    bool m_isValid = false; ///< 数据库可用性标识
};

#endif // DBMANAGER_H
