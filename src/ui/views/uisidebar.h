#ifndef UISIDEBAR_H
#define UISIDEBAR_H

#include "playlist_info.h"
#include "timer.h"
#include "timing.h"
#include "create_playlist_dialog.h"
#include <QWidget>
#include <QButtonGroup>
#include <QDialog>
#include <QMenu>

namespace Ui {
class UISideBar;
class UICreatePlaylistDialog;
class UITiming;
}


Q_DECLARE_METATYPE(PlayListInfo)

class UISideBar : public QWidget
{
    Q_OBJECT

public:
    explicit UISideBar(QWidget *parent = nullptr);
    ~UISideBar();

    // 查找歌单
    QAbstractButton* findPlaylist(int playlist_id);

    // 返回指定歌单的序号
    int findPlaylistOrder(QAbstractButton *btn);

    // 根据歌单序号查找歌单按钮
    QPushButton* findPlaylistBtn(int order);

    // 根据主页面类型返回对应按钮
    QPushButton* getSideBtnOfPage(int page);

    // 返回选择的歌单序号
    int currentSelectPlaylist();

    // 更新歌单封面图片
    void updatePlaylistCover(int songlist_id, int song_id, const QString &path);

    // 设置歌单封面图片（初始设置）
    void setPlaylistCover(int song_id, const QString& path, QAbstractButton* btn);

    // 显示倒计时
    void showCountdownBtn();

    // 隐藏倒计时
    void hideCountdownBtn();

    // 批量创建歌单
    void createPlaylists(const QList<PlayListInfo> &list);

    // 删除歌单
    void deletePlaylist(int playlist_id);

    // 创建歌单
    void createPlaylist(const PlayListInfo &info);

signals:
    // 页面切换信号
    void pageChanged(int pageIndex);

    // 歌单点击信号
    void playlistClicked(const PlayListInfo &info);

    // 刷新歌单列表
    void playlistUpdated(int playlist_id);

    // 添加歌单信号
    PlayListInfo playlistAdded(const QString &name);

    // 歌单删除信号
    void playlistDeleted(int playlist_id);

    // 歌单播放信号
    void playlistPlayed();

    // 歌单名字更改信号
    void playlistNameChanged(const QString& name, int playlist_id);

    // 播放与暂停信号
    void songPlayedOrPaused();

private slots:

    // 添加歌单事件
    void on_addPlaylistBtn_clicked();

    // 定时事件
    void on_timingBtn_clicked();

private:
    // 连接信号与槽
    void connectSignals();

    // 初始化侧边栏按钮
    void initSideBtn();

    // 切换到本地页面
    void toggleToLocalPage(bool checked);

    // 切换到收藏页面
    void toggleToCollectPage(bool checked);

    // 切换到云端页
    void toggleToNetworkPage(bool checked);

    // 初始化右键菜单
    void initContextMenu();

    // 点击菜单栏
    void triggerMenu(QAction *action);

    Ui::UISideBar *ui;

    QButtonGroup *m_pageBtnGroup{}; // 按钮组

    QMenu *m_contextMenu{}; // 右键菜单

    int m_currentSelectPlaylist = -1; // 当前选中的歌单序号

    Timing *m_timing;
    CreatePlaylistDialog *m_createPlaylisDialog;
};

#endif // UISIDEBAR_H
