#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "player_controller.h"
#include "song_manager.h"
#include "uimain.h"
#include "uisidebar.h"
#include "uisearch.h"
#include "appmediator.h"
#include "music_detail_widget.h"
#include <QWidget>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QSystemTrayIcon>
#include <QMenu>

class MusicPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit MusicPlayer(QWidget *parent = nullptr);

    void initLayout(); // 初始化布局
    void saveSettings(); // 保存配置
    void loadSettings(); // 加载配置
    void createTrayIcon(); // 创建系统托盘图标

protected:
    void closeEvent(QCloseEvent *event) override; // 程序窗口关闭事件
    void resizeEvent(QResizeEvent *event) override; // 程序窗口更改
    void keyPressEvent(QKeyEvent *event) override; // 快捷键

private:
    AppMediator *m_mediator{}; // 应用中介者
    PlayerController *m_controller{}; // 播放控制器
    SongManager *m_songManager{}; // 列表管理器
    UIMain *m_uiMain{}; // 主界面
    UISideBar *m_uiSideBar{}; // 侧边栏
    UISearch *m_uiSearch{}; // 搜索栏
    MusicDetailWidget *m_detailWidget{}; // 全屏页
    QSystemTrayIcon *m_trayIcon{}; // 系统托盘图标
    QMenu *m_trayMenu{}; // 托盘菜单栏

    bool m_isQuitting = false; // 是否退出
};

#endif // MUSICPLAYER_H
