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

    bool setSource(const QModelIndex &index); // 设置播放源
    void play(bool autoPlay); // 播放音乐

    void setPlayProgress(int value); // 设置播放进度
    void setVolume(int value); // 设置音量

    QMediaPlayer* mediaPlayer(); // 返回播放器对象
    QAudioOutput* audioOutput(); //  返回音频输出对象
    int volume(); // 返回音量
    int position(); // 返回当前播放位置

signals:
    void playbackStarted(bool autoPlay); // 播放开始信号
    void playbackError(const QString &msg); // 播放错误信号

public slots:
    void playMusic(const QModelIndex &index,  bool autoPlay); // 播放音乐
    void playOrPause(); // 自动播放/暂停
    void skipMusic(const QModelIndex &index); // 上/下一首音乐

private:
    QMediaPlayer *m_player{};
    QAudioOutput *m_audioOutput{};
};


#endif // PLAYER_CONTROLLER_H
