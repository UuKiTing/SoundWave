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

    // 设置播放源
    bool setSource(const QModelIndex &index);

    // 播放歌曲
    void play(bool autoPlay);

    // 设置歌曲播放进度
    void setPlayProgress(int value);

    // 返回当前播放进度
    int playProgress();

    // 设置音量
    void setVolume(int value);

    // 返回当前音量
    int volume();

signals:
    // 播放错误信号
    void playbackError(const QString &msg);

    // ========= QMediaPlayer的转发信号 =========

    // 音量变化信号
    void volumeChanged(float volume);

    // 播放状态变化信
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);

    // 媒体状态变化信号
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

    // 音频总时长变化信号
    void durationChanged(qint64 duration);

    // 播放进度变化信号
    void positionChanged(qint64 position);

public slots:
    // 播放歌曲
    void playSong(const QModelIndex &index,  bool autoPlay);

    // 自动播放与暂停
    void playOrPause();

    // 上、下一首歌曲
    void skipSong(const QModelIndex &index);

private:
    void connectSignals(); // 连接信号和槽
    void forwardSignals(); // 转发型号

    QMediaPlayer *m_player{};
    QAudioOutput *m_audioOutput{};
};


#endif // PLAYER_CONTROLLER_H
