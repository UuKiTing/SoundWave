#ifndef APPMEDIATOR_H
#define APPMEDIATOR_H

#include "uimain.h"
#include "uisearch.h"
#include "uisidebar.h"
#include "player_controller.h"
#include "song_manager.h"
#include "music_detail_widget.h"
#include <QObject>
#include <QModelIndex>

class AppMediator : public QObject
{
    Q_OBJECT
public:
    explicit AppMediator(QObject *parent = nullptr);

    void connectSignal();
    void setPlayer(PlayerController *controller);
    void setListManager(SongManager *listManager);
    void setUIMain(UIMain *uiMain);
    void setUISideBar(UISideBar *uiSideBar);
    void setUISearch(UISearch *uiSearch);
    void setDetailWidget(MusicDetailWidget *detailWidget);

signals:

private slots:
    void skipMusic(bool isNext); // isNext：为true时是下一首歌曲，反之上一首歌曲
    void playMusic(const QModelIndex &index, bool autoPlay); // 播放歌曲

private:
    void playConnect(); // 播放功能
    void progressSliderConnect(); // 进度条
    void volumeSliderConnect(); //  音量
    void pageConnect(); // 页面切换
    void collectConnect(); // 音乐收藏
    void searchConnect(); // 搜索框
    void playlistConnect(); // 歌单
    void detailWidgetConnect(); // 详情页

    void collectSong(bool isCollect, const QModelIndex &index = QModelIndex()); // 收藏音乐

    PlayerController *m_controller{}; // 播放控制器
    SongManager *m_songManager{}; // 音乐列表管理器
    UIMain *m_uiMain{}; // 主界面
    UISideBar *m_uiSideBar{}; // 侧边栏
    UISearch *m_uiSearch{}; // 搜索栏
    MusicDetailWidget *m_detailWidget{}; // 全屏播放页

    bool m_isDragging = false;// 标志位：是否关闭播放器进度和进度条滑块的同步
};

#endif // APPMEDIATOR_H
