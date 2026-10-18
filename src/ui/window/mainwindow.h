#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "uimain.h"
#include "uisidebar.h"
#include "uisearch.h"
#include "music_detail_widget.h"
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMoveEvent>
#include <QResizeEvent>

class SongManager;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    bool initialize();
    void initLayout();

    UIMain *uiMain();
    UISideBar *uiSideBar();
    UISearch *uiSearch();
    MusicDetailWidget *detailWidget();

    bool createTrayIcon();

protected:
    // 程序窗口关闭事件
    void closeEvent(QCloseEvent *event) override;

    // 程序窗口更改
    void resizeEvent(QResizeEvent *event) override;

    // 快捷键
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void settingsSaved();

private:
    UIMain *m_uiMain{}; // 主界面
    UISideBar *m_uiSideBar{}; // 侧边栏
    UISearch *m_uiSearch{}; // 搜索栏
    MusicDetailWidget *m_detailWidget{}; // 歌曲全屏页

    QSystemTrayIcon *m_trayIcon{};  // 系统托盘图标
    QMenu *m_trayMenu{}; // 托盘菜单栏

    bool m_isQuitting = false; // 是否退出
};

#endif // MAINWINDOW_H
