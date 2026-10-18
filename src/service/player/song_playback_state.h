#ifndef SONG_PLAYBACK_STATE_H
#define SONG_PLAYBACK_STATE_H

#include "page.h"
#include "play_mode.h"
#include <QObject>
#include <QModelIndex>

// 歌曲播放状态管理
class SongPlaybackState : public QObject
{
    Q_OBJECT
public:
    explicit SongPlaybackState(QObject *parent = nullptr);

    // 记录当前播放歌曲所在播放列表的行数
    void setListRows(int rows);

    // 返回当前播放歌曲所在播放列表的所有歌曲行号
    int listRows();

    // 记录哪个歌单处于播放状态
    void setCurrentPlaylist(int playlist_id);

    // 取处于播放状态的歌单序号
    int currentPlaylist();

    // 更改播放模式
    void changePlayMode();

    // 获取当前播放模式
    PlayMode playMode();

    // 置播放模式
    void setPlayMode(PlayMode mode);

    // 记录当前播放的主页
    void setCurrentPage(MainPage page);

    // 返回当前播放的主页面
    MainPage currentPage();

    // 设置上/下一首歌曲的行号
    int setNextRow(bool isNext);

    // 置当前播放歌曲的行号
    void setCurrentRow(int row);

    // 返回当前播放歌曲的行号
    int currentRow();

    // 设置当前播放歌曲的代理模型id
    void setProxyId(ProxyId id);

    // 返回当前播放歌曲的代理模型id
    ProxyId proxyId();

    int calculateLoopRow(int row, bool isNext);

    int calculateRandRow(int currentRow);

private:
    PlayMode m_playMode = PlayMode::Loop; // 播放模式

    int m_listRows = 0; // 当前播放列表的行数

    MainPage m_currentPage = MainPage::Local; // 当前播放歌曲所在的主页面

    int m_currentPlaylist = -1; // 当前播放歌曲所在的歌单序号

    int m_currentRow = 0; // 当前播放歌曲的行号

    ProxyId m_proxyId = ProxyId::Local; // 当前播放歌曲的代理模型id
};

#endif // SONG_PLAYBACK_STATE_H
