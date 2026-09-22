#ifndef UISIDEBAR_H
#define UISIDEBAR_H

#include "playlist_info.h"
#include "timer.h"
#include <QWidget>
#include <QButtonGroup>
#include <QDialog>
#include <QMenu>

namespace Ui {
class UISideBar;
class UiDialog;
class UITiming;
}


Q_DECLARE_METATYPE(PlayListInfo)

class UISideBar : public QWidget
{
    Q_OBJECT

public:
    explicit UISideBar(QWidget *parent = nullptr);
    ~UISideBar();

    /** @brief 查找歌单 */
    QAbstractButton* findPlaylist(int playlist_id);

    /** @brief 返回指定歌单的序号 */
    int findPlaylistOrder(QAbstractButton *btn);

    /** @brief 根据歌单序号查找歌单按钮 */
    QPushButton* findSonglistBtn(int order);

    /** @brief 根据主页面类型返回对应按钮 */
    QPushButton* getSideBtnOfPage(int page);

    /** @brief 返回选择的歌单序号 */
    int playlistSelectNumber();

    /** @brief 更新歌单封面图片 */
    void updatePlaylistCover(int songlist_id, int song_id, const QString &path);

    /** @brief 设置歌单封面图片（初始设置） */
    void setPlaylistCover(int song_id, const QString& path, QAbstractButton* btn);

    /** @brief 设置倒计时的可见性 */
    void setCountDownVisible(bool visible);

signals:
    /** @brief 页面切换信号 */
    void pageChanged(int pageIndex);

    /** @brief 歌单点击信号 */
    void playlistClicked(const PlayListInfo &info);

    /** @brief 刷新歌单列表 */
    void playlistUpdated(const QSet<int> &songIds);

    /** @brief 歌单创建信号 */
    void playlistCreated(const PlayListInfo &info);

    /** @brief 歌单删除信号 */
    void playlistDeleted(int playlist_id);

    /** @brief 歌单播放信号 */
    void playlistPlayed();

    /** @brief 歌单名字更改信号 */
    void playlistNameChanged(const QString& name, int playlist_id);

    /** @brief 播放与暂停信号 */
    void songPlayedOrPaused();


private slots:

    /** @brief 添加歌单事件 */
    void on_addSongBtn_clicked();

    /** @brief 定时事件 */
    void on_timingBtn_clicked();

private:
    /** @brief 连接信号与槽 */
    void connectSignals(); //

    /** @brief 初始化侧边栏按钮 */
    void initSideBtn();

    /** @brief  */
    void initTimingPage();

    /** @brief 切换到本地页面 */
    void toggleToLocalPage(bool checked);

    /** @brief 切换到收藏页面 */
    void toggleToCollectPage(bool checked);

    /** @brief 切换到云端页 */
    void toggleToNetworkPage(bool checked);

    /** @brief 创建歌单 */
    void createPlaylist(const PlayListInfo &info);

    /** @brief 初始化右键菜单 */
    void initContextMenu(); //

    /** @brief 清空输入对话框 */
    void clearInputBox(); //

    /** @brief 点击菜单栏 */
    void triggerMenu(QAction *action);

    /** @brief 删除歌单 */
    void deletePlaylist(QAbstractButton *btn, int playlist_id);

    Ui::UISideBar *ui;
    Ui::UiDialog *ui_dialog; ///< 创建歌单模态框UI对象
    Ui::UITiming *ui_timing;

    QDialog *m_dialog{}; ///< ui_dialog的父窗口，用于显示歌单模态框
    QDialog *m_timingDialog{}; ///< ui_timing的父窗口，用于显示定时模态框

    QButtonGroup *m_pageBtnGroup{}; ///< 按钮组
    QButtonGroup *m_timerCheckBoxGroup{}; ///< 复选框按钮组

    QMenu *m_contextMenu{}; ///< 右键菜单

    int m_playlistSelectNumber = -1; ///< 当前选中的歌单序号

    Timer *m_mainTimer;
    Timer *m_updateTimer;

};

#endif // UISIDEBAR_H
