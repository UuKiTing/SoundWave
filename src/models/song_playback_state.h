#ifndef SONG_PLAYBACK_STATE_H
#define SONG_PLAYBACK_STATE_H

#include "page.h"
#include "play_mode.h"
#include <QObject>
#include <QModelIndex>

/**
 * @brief 歌曲播放状态管理
 */
class SongPlaybackState : public QObject
{
    Q_OBJECT
public:
    explicit SongPlaybackState(QObject *parent = nullptr);

    /**
     * @brief 记录当前播放歌曲所在播放列表的所有歌曲行号
     *        因为行号从0开始递增，所以可直接用一个整数表示
     * @param rows 行数
     */
    void setListRows(int rows);

    /**
     * @brief 返回当前播放歌曲所在播放列表的所有歌曲行号
     *        因为行号从0开始递增，所以可直接用一个整数表示
     * @return int 行数
     */
    int listRows();

    /**
     * @brief 记录哪个歌单处于播放状态
     * @param number 歌单序号
     */
    void setPlaylistPlayingNumber(int number);

    /**
     * @brief 获取处于播放状态的歌单序号
     * @return int 歌单序号
     */
    int playlistPlayingNumber();

    /**
     * @brief 更改播放模式
     * @note 自动循环更改 Loop -> Random -> Single
     */
    void changePlayMode();

    /**
     * @brief 获取当前播放模式
     * @return PlayMode 播放模式
     */
    PlayMode playMode();

    /**
     * @brief 设置播放模式
     * @param mode 播放模式
     */
    void setPlayMode(PlayMode mode);

    /**
     * @brief 记录当前播放的主页面
     * @param page 主页面类型
     */
    void setCurrentPage(MainPage page);

    /**
     * @brief 返回当前播放的主页面
     * @return MainPage 主页面类型
     */
    MainPage currentPage();

    /**
     * @brief 设置上/下一首歌曲的行号
     * @param isNext 是否为下一首歌曲，ture 为下一首，false 为上一首
     * @return int 上/下一首歌曲的行号
     */
    int setNextRow(bool isNext);

    /**
     * @brief 设置当前播放歌曲的行号
     * @param row 行号
     */
    void setCurrentRow(int row);

    /**
     * @brief 返回当前播放歌曲的行号
     * @return int 行号
     */
    int currentRow();

    /**
     * @brief 设置当前播放歌曲的代理模型id
     * @param id 代理模型id
     */
    void setProxyId(ProxyId id);

    /**
     * @brief 返回当前播放歌曲的代理模型id
     * @return ProxyId 代理模型
     */
    ProxyId proxyId();
signals:


private:
    PlayMode m_playMode = PlayMode::Loop; ///< 播放模式

    int m_listRows = 0; ///< 当前播放列表的行数

    MainPage m_currentPage = MainPage::Local; ///< 当前播放歌曲所在的主页面

    int m_playlistPlayingNumber = -1; ///< 当前播放歌曲所在的歌单序号

    int m_currentRow = 0; ///< 当前播放歌曲的行号

    ProxyId m_proxyId = ProxyId::Local; ///< 当前播放歌曲的代理模型id
};

#endif // SONG_PLAYBACK_STATE_H
