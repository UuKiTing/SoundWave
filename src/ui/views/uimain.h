#ifndef UIMAIN_H
#define UIMAIN_H

#include "play_mode.h"
#include "playlist_info.h"
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

    // 设置播放按钮图标
    void setPlayBtnIcon(QMediaPlayer::PlaybackState state);

    // 设置当前的播放时长
    void setCurDuration(qint64 position);

    // 设置总的播放时长
    void setTotalDuration(const QString &durationString);

    // 设置进度条的范围
    void setProgressSliderRange(qint64 duration);

    // 设置进度值
    void setProgressValue(qint64 position);

    // 拖拽进度
    void progressDragging();

    // 没有拖拽进度
    void progressNodragging();

    // 设置当前播放的音乐封面
    void setCoverIcon(int song_id, const QString &path);

    // 设置音量大小
    void setVolumeValue(float volume);

    // 设置歌曲标题和作者
    void setTitleAndArtist(const QString &title, const QString &artist);

    // 设置视图的当前模型索引
    void setCurrentIndex(const QModelIndex &index);

    // 设置歌曲列表名称
    void setPlaylistName(const QString &name);

    // 设置歌曲列表封面
    void setSonglistCover(int song_id, const QString &path);

    // 设置音乐播放样式
    void setPlayStyle(const QModelIndex &index);

    // 切换页面
    void switchStackedWidget(int pageIndex);

    // 收藏状态切换
    void collectStatusToggle(bool checked);

    // 收藏图标切换
    void collectIconToggle(bool isFavo);

    // 获取本地视图
    QListView* localListView();

    // 获取收藏视图
    QListView* collectListView();

    // 获取歌单视图
    QListView* playlistView();

    // 获取云端视图
    QListView* remoteListView();

    // 获取进度条
    QSlider* progressSlider();

    // 获取音量条
    QSlider* volumeSlider();

    // 获取进度条的值
    int progressValue();

    // 获取当前所在页
    int currentPage();

    // 获取控制栏
    QFrame* controlBar();

    // 获取模型的列表行数
    int geListRows(const QAbstractItemModel *model);

    // 获取歌单播放按钮
    QPushButton* playlistBtn();

    // 后驱当前所在的页的视图
    QListView* currentListView();

signals:
    // 歌曲播放信号
    void songPlayed(const QModelIndex &index, bool autoPlay);

    // 播放与暂停信号
    void songPlayedOrPaused();

    // 上/下一首播放信号
    void songSkipped(bool isNext);

    // 播放模式更改信号
    void playModeChanged();

    // 歌曲收藏信号
    void songCollected(bool isCollect, const QModelIndex &index = QModelIndex());

    // 显示详情页信号
    void detailWidgetShowed(bool isVisible);

    // 进度条按下信号
    void sliderPressed();

    // 进度条释放信号
    void sliderReleased();

    // 进度值更改信号
    void progressChanged(int value);

public slots:
     // 双击播放
    void doubleClickPlay(const QModelIndex &index, bool autoPlay);

     // 更改播放模式
    void changePlayMode(PlayMode mode);

     // 播放上/下一首歌曲
    void skipMusic(bool isNext);

    void updatePlaylist(const PlayListInfo &info);

private slots:
    // 本地视图双击事件
    void on_localListView_doubleClicked(const QModelIndex &index);

    // 收藏视图双击事件
    void on_collectListView_doubleClicked(const QModelIndex &index);

    // 播放与暂停事件
    void on_playBtn_clicked();

    // 播放模式切换事件
    void on_modeBtn_clicked();

    // 播放下一首歌曲事件
    void on_nextBtn_clicked();

    // 播放上一首歌曲事件
    void on_lastBtn_clicked();

    // 显示音量条事件
    void on_volumeBtn_clicked();

    // 收藏事件
    void on_loveBtn_clicked(bool checked);

    // 显示歌曲详情页事件
    void on_coverBtn_toggled(bool checked);

    // 歌单视图双击事件
    void on_playlistView_doubleClicked(const QModelIndex &index);

    // 播放歌单歌曲事件
    void on_playlistBtn_clicked();

    // 云端视图双击事件
    void on_remoteListView_doubleClicked(const QModelIndex &index);

    // 播放收藏歌曲列表事件
    void on_collectlistBtn_clicked();

    // 播放本地歌曲列表事件
    void on_locallistBtn_clicked();

private:
    // 连接信号槽
    void connectSignals();

    // 转发信号
    void forwardSignals();

    // 初始化音量条
    void initVolumeMenu();

    // 播放歌曲列表
    void playSonglist(QAbstractItemModel *model);

    Ui::UIMain *ui;
    StyleItemDelegate *m_delegate{}; // 自定义代理
    QMenu *m_volumeMenu{}; // 音量菜单
    QSlider *m_volumeSlider{}; // 音量滑块

    bool m_isDragging = false;// 标志位：是否关闭播放器进度和进度条滑块的同步
};

#endif // UIMAIN_H
