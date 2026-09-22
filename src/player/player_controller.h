#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QModelIndex>

class PlayerController : public QObject
{
    Q_OBJECT
public:
    explicit PlayerController(QObject *parent = nullptr);

    /**
     * @brief 设置播放源
     * @param index 模型索引
     * @return bool 设置成功返回 true, 失败则返回 false
     */
    bool setSource(const QModelIndex &index);

    /**
     * @brief 播放歌曲
     * @param autoPlay true=自动播放, false=不自动播放
     */
    void play(bool autoPlay);

    /**
     * @brief 设置歌曲播放进度
     * @param value 进度值
     */
    void setPlayProgress(int value);

    /**
     * @brief 返回当前播放进去
     * @return int 进度值
     */
    int playProgress();

    /**
     * @brief 设置音量
     * @param value 音量值
     */
    void setVolume(int value);

    /**
     * @brief 返回当前音量
     * @return int 音量值
     */
    int volume();

signals:
    /**
     * @brief 播放错误信号
     * @param msg 错误信息
     */
    void playbackError(const QString &msg);

    // ========= QMediaPlayer的转发信号 =========

    /** @brief 音量变化信号 */
    void volumeChanged(float volume);

    /** @brief 播放状态变化信 */
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);

    /** @brief 媒体状态变化信号 */
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

    /** @brief 音频总时长变化信号 */
    void durationChanged(qint64 duration);

    /** @brief 播放进度变化信号 */
    void positionChanged(qint64 position);

public slots:
    /**
     * @brief 播放歌曲
     * @param index 模型索引
     * @param autoPlay true=自动播放, false=不自动播放
     */
    void playSong(const QModelIndex &index,  bool autoPlay);

    /**
     * @brief 自动播放与暂停
     */
    void playOrPause();

    /**
     * @brief 上/下一首歌曲
     * @param index 模型索引
     */
    void skipSong(const QModelIndex &index);

private:
    QMediaPlayer *m_player{};
    QAudioOutput *m_audioOutput{};
};


#endif // PLAYER_CONTROLLER_H
