#ifndef UIMAIN_H
#define UIMAIN_H

#include "global.h"
#include "styleitem_delegate.h"
#include <QWidget>
#include <QListView>
#include <QMediaPlayer>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QAbstractProxyModel>
#include <QVector>
#include <QSortFilterProxyModel>
#include <QPushButton>

namespace Ui {
class UIMain;
}

class UIMain : public QWidget
{
    Q_OBJECT

public:
    explicit UIMain(QWidget *parent = nullptr);
    ~UIMain();

    void setModel(QAbstractItemView *view, QAbstractItemModel *model); // 为视图设置模型
    void setPlayBtnIcon(QMediaPlayer::PlaybackState state); // 设置播放按钮图标
    void setCurDuration(qint64 position); // 设置当前的播放时长
    void setTotalDuration(const QString &durationString); // 设置总的播放时长
    void setProgressSliderRange(qint64 duration); // 设置进度条的范围
    void setProgressValue(qint64 position); // 设置进度值
    void setCoverIcon(const QString &path); // 设置当前播放的音乐封面
    void setVolumeValue(float volume); // 设置音量大小
    void setTitleAndArtist(const QString &title, const QString &artist); // 设置音乐标题和作者
    void setCurrentIndex(const QModelIndex &index); // 设置音乐列表视图的index
    void setPlaylistName(const QString &name); // 设置歌曲列表名称
    void setSonglistCover(const QString &path); // 设置歌曲列表封面
    void setPlayStyle(const QModelIndex &index); // 设置音乐播放样式

    void switchStackedWidget(int pageIndex); // 页面切换
    void collectStatusToggle(bool checked); // 收藏状态切换
    void collectIconToggle(bool isFavo); // 收藏图标切换

    QListView* listView(); // 获取音乐列表视图
    QListView* collectListView(); // 获取收藏音乐列表视图
    QListView* songListView(); // 获取歌单列表视图
    QListView* remoteListView();
    QSlider* progressSlider(); // 获取进度条
    QSlider* volumeSlider(); // 获取音量条
    int progressValue(); // 获取进度条的值
    int currentPage();
    QFrame* controlBar(); // 获取控制栏
    int getPlaylistRows(const QAbstractItemModel *model);
    QPushButton* playlistBtn();
    QListView* currentListView();
    QString coverPath();


signals:
    void songPlayed(const QModelIndex &index, bool autoPlay); // 播放歌曲
    void songPlayedOrPaused(); // 播放与暂停
    void songSkipped(bool isNext); // 上/下一首音乐
    void modeChanged(); // 播放模式更改
    void collected(const QModelIndex &index = QModelIndex()); // 收藏音乐
    void cancelCollected(const QModelIndex &index = QModelIndex()); // 取消收藏音乐
    void showDetailWidget(bool isVisible); // 显示音乐详情页

public slots:
    void doubleClickPlay(const QModelIndex &index, bool autoPlay); // 音乐列表双击事件
    void changePlayMode(PlayMode mode); // 播放模式更改
    void skipMusic(bool isNext); // 上/下一首按钮点击事件

private slots:
    void on_listView_doubleClicked(const QModelIndex &index); // 音乐列表双击事件

    void on_collectListView_doubleClicked(const QModelIndex &index); // 收藏音乐列表双击事件

    void on_playBtn_clicked(); // 播放/暂停按钮点击事件

    void on_modeBtn_clicked(); // 播放模式按钮点击事件

    void on_nextBtn_clicked(); // 上一首/下一首按钮点击事件

    void on_lastBtn_clicked(); // 上一首/下一首按钮点击事件

    void on_volumeBtn_clicked(); // 音量按钮点击事件

    void on_loveBtn_clicked(bool checked); // 收藏按钮点击事件

    void on_coverBtn_toggled(bool checked); // 封面切换事件

    void on_songListView_doubleClicked(const QModelIndex &index);

    void on_playlistBtn_clicked();

    void on_remoteListView_doubleClicked(const QModelIndex &index);

private:
    void connectSignal(); // 连接信号槽
    void initVolumeMenu(); // 初始化音量条

    Ui::UIMain *ui;
    StyleItemDelegate *m_delegate{}; // 自定义代理
    QMenu *m_volumeMenu{}; // 音量菜单
    QSlider *m_volumeSlider{}; // 音量滑块

    QString m_cover;
};

#endif // UIMAIN_H
