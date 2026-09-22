#ifndef UIMAIN_H
#define UIMAIN_H

#include "play_mode.h"
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

    /** @brief 为视图设置模型 */
    void setModel(QAbstractItemView *view, QAbstractItemModel *model);

    /** @brief 设置播放按钮图标 */
    void setPlayBtnIcon(QMediaPlayer::PlaybackState state);

    /** @brief 设置当前的播放时长 */
    void setCurDuration(qint64 position);

    /** @brief 设置总的播放时长  */
    void setTotalDuration(const QString &durationString);

    /** @brief 设置进度条的范围  */
    void setProgressSliderRange(qint64 duration);

    /** @brief 设置进度值  */
    void setProgressValue(qint64 position);

    /** @brief 设置当前播放的音乐封面  */
    void setCoverIcon(int song_id, const QString &path);

    /** @brief 设置音量大小  */
    void setVolumeValue(float volume);

    /** @brief 设置歌曲标题和作者 */
    void setTitleAndArtist(const QString &title, const QString &artist);

    /** @brief 设置视图的当前模型索引 */
    void setCurrentIndex(const QModelIndex &index);

    /** @brief 设置歌曲列表名称 */
    void setPlaylistName(const QString &name);

    /** @brief 设置歌曲列表封面 */
    void setSonglistCover(int song_id, const QString &path);

    /** @brief 设置音乐播放样式 */
    void setPlayStyle(const QModelIndex &index);

    /** @brief 切换页面 */
    void switchStackedWidget(int pageIndex);

    /** @brief 收藏状态切换 */
    void collectStatusToggle(bool checked);

    /** @brief 收藏图标切换 */
    void collectIconToggle(bool isFavo);

    /** @brief 获取本地视图 */
    QListView* localListView(); //

    /** @brief 获取收藏视图 */
    QListView* collectListView();

    /** @brief 获取歌单视图 */
    QListView* playlistView();

    /** @brief 获取云端视图 */
    QListView* remoteListView();

    /** @brief 获取进度条 */
    QSlider* progressSlider();

    /** @brief 获取音量条 */
    QSlider* volumeSlider();

    /** @brief 获取进度条的值 */
    int progressValue();

    /** @brief 获取当前所在页 */
    int currentPage();

    /** @brief 获取控制栏 */
    QFrame* controlBar(); //

    /** @brief 获取模型的列表行数 */
    int geListRows(const QAbstractItemModel *model);

    /** @brief 获取歌单播放按钮 */
    QPushButton* playlistBtn();

    /** @brief 后驱当前所在的页的视图 */
    QListView* currentListView();

signals:
    /** @brief 歌曲播放信号 */
    void songPlayed(const QModelIndex &index, bool autoPlay);

    /** @brief 播放与暂停信号 */
    void songPlayedOrPaused(); //

    /** @brief 上/下一首播放信号 */
    void songSkipped(bool isNext);

    /** @brief 播放模式更改信号 */
    void playModeChanged(); //

    /** @brief 歌曲收藏信号 */
    void songCollected(bool isCollect, const QModelIndex &index = QModelIndex());

    /** @brief 显示详情页信号 */
    void detailWidgetShowed(bool isVisible);

    /** @brief 进度条按下信号 */
    void sliderPressed();

    /** @brief 进度条释放信号 */
    void sliderReleased();

public slots:
     /** @brief 双击播放 */
    void doubleClickPlay(const QModelIndex &index, bool autoPlay);

     /** @brief 更改播放模式 */
    void changePlayMode(PlayMode mode);

     /** @brief 播放上/下一首歌曲 */
    void skipMusic(bool isNext);

private slots:
    /** @brief  本地视图双击事件 */
    void on_localListView_doubleClicked(const QModelIndex &index);

    /** @brief  收藏视图双击事件 */
    void on_collectListView_doubleClicked(const QModelIndex &index);

    /** @brief  播放与暂停事件*/
    void on_playBtn_clicked();

    /** @brief  播放模式切换事件 */
    void on_modeBtn_clicked();

    /** @brief  播放下一首歌曲事件 */
    void on_nextBtn_clicked();

    /** @brief  播放上一首歌曲事件 */
    void on_lastBtn_clicked();

    /** @brief  显示音量条事件 */
    void on_volumeBtn_clicked();

    /** @brief  收藏事件 */
    void on_loveBtn_clicked(bool checked); // 收藏按钮点击事件

    /** @brief  显示歌曲详情页事件 */
    void on_coverBtn_toggled(bool checked); // 封面切换事件

    /** @brief  歌单视图双击事件 */
    void on_playlistView_doubleClicked(const QModelIndex &index);

    /** @brief  播放歌单歌曲事件 */
    void on_playlistBtn_clicked();

    /** @brief  云端视图双击事件 */
    void on_remoteListView_doubleClicked(const QModelIndex &index);

    /** @brief  播放收藏歌曲列表事件 */
    void on_collectlistBtn_clicked();

    /** @brief  播放本地歌曲列表事件 */
    void on_locallistBtn_clicked();

private:
    /** @brief 连接信号槽  */
    void connectSignal();

    /** @brief 初始化音量条 */
    void initVolumeMenu();

    /** @brief 播放歌曲列表 */
    void playSonglist(QAbstractItemModel *model);

    Ui::UIMain *ui;
    StyleItemDelegate *m_delegate{}; // 自定义代理
    QMenu *m_volumeMenu{}; // 音量菜单
    QSlider *m_volumeSlider{}; // 音量滑块
};

#endif // UIMAIN_H
