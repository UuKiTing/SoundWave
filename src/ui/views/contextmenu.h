#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "song_info.h"
#include "playlist_info.h"
#include "http_request.h"
#include <QObject>
#include <QMenu>
#include <QPoint>
#include <QListView>

/**
 * @brief 右键菜单管理
 * @note 单例模式
 */
class ContextMenu : public QWidget
{
    Q_OBJECT
public:
    ///< 禁用拷贝构造、赋值
    ContextMenu(const ContextMenu&) = delete;
    ContextMenu& operator=(const ContextMenu&) = delete;

    /** @brief 返回ContextMenu单例对象 */
    static ContextMenu& getInstance();

    /**
     * @brief 显示右键菜单
     * @param view 视图
     * @param pos 位置
     */
    void show(QListView *view, const QPoint &pos);

    /**
     * @brief 更新右键菜单中的歌单名称
     * @param name 名称
     * @param palylist_id 歌单Id
     */
    void updatePlaylistName(const QString &name, int palylist_id);


    /**  @brief 设置 */
    void setActionIcon(QAction *action, const QString &filePath);

signals:
    /**
     * @brief 右键菜单中的歌单封面图片更新信号
     * @param playlist_id 歌单id
     * @param path 封面图片路径
     */
    void playlistCoverUpdated(int playlist_id, int song_id, const QString& path);

    /**
     * @brief 歌曲列表封面图片更新信号
     * @param path 封面图片路径
     */
    void songlistCoverUpdated(int song_id, const QString& path);

    /**
     * @brief 歌曲删除信号
     * @param song_id 歌曲Id
     * @param row 行号
     */
    void songRemoved(int song_id, int row);

    /**
     * @brief 歌曲播放信号
     * @param row 行号
     */
    void songPlayed(int row);

    /**
     * @brief 歌曲收藏信号
     * @param isCollect true=收藏, false=取消收藏
     * @param index 模型索引
     */
    void songCollected(bool isCollect, const QModelIndex &index);

    /** @brief 进度值更改 */
    void progressChanged(const QModelIndex &index, int progressValue);

    void songAppended(SongInfo song);

public slots:
    /**
     * @brief 添加歌曲到歌单中
     * @param action QAction通过data()获取歌单Id
     * @return bool 添加成功返回true, 失败则返回false
     */
    bool addSongToPlaylist(QAction* action);

    /**
     * @brief 移动歌曲到指定歌单中
     * @param action QAction 通过data()获取歌单Id
     */
    void moveSongToPlaylist(QAction* action); // 移动歌曲到被的歌单中

    /**
     * @brief 添加歌单
     * @param info 歌单元数据
     */
    void addPlaylist(const PlayListInfo &info);

    /**
     * @brief 删除歌单
     * @param playlist_id 歌单Id
     */
    void removePlaylist(int playlist_id); // 删除右键菜单中的歌单

private:
    ContextMenu();
    ~ContextMenu();

    void init(); ///< 初始化

    /**
     * @brief 为右键菜单添加歌单
     * @param info 歌单元数据
     * @return 创建好的QAction*指针
     */
    QAction* createAction(const PlayListInfo &info);

    /**
     * @brief 同步两个QAction的显示状态
     * @param src 源 action, 其状态被同步出去
     * @param dist 目标 action, 接受 src 的状态更新
     */
    void syncConnect(QAction *src, QAction *dist);

    /** * @brief 设置歌单的启用性 */
    void setPlaylistEnabled();

    /** @brief 设置移动菜单的可见性 */
    void setMoveMenuVisible(QListView *view);

    /** @brief 设置下载歌曲作的可见性 */
    void setDownloadActionVisible(QListView *view);

    /** @brief 设置收藏歌曲操作的可见性 */
    void setLoveActionIcon(bool isFavo);

    /** @brief 设置删除歌曲操作的可见性 */
    void setDelActionVisible(QListView *view);

    /** @brief 下载歌曲 */
    void downloadSong(QModelIndex index);

    QMenu *m_contextMenu = nullptr;
    QMenu *m_addToMenu = nullptr;
    QMenu *m_moveToMenu = nullptr;

    QAction *m_playAction = nullptr;
    QAction *m_loveAction = nullptr;
    QAction *m_delAction = nullptr;
    QAction *m_downloadAction = nullptr;

    QModelIndex m_curIndex = QModelIndex();

    bool m_syncing = false; ///< 同步标识，用于syncConnect函数
};

#endif // CONTEXTMENU_H
