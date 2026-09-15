#include "player_controller.h"
#include "global.h"
#include "logging.h"
#include <QFile>

PlayerController::PlayerController(QObject *parent)
    : QObject{parent}
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);

    m_player->setAudioOutput(m_audioOutput);

    setVolume(20);

    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error err){
        Q_UNUSED(err);
        emit playbackError(m_player->errorString());
    });

}


bool PlayerController::setSource(const QModelIndex &index)
{
    QString filePath = index.data(Roles::FilePath).toString();
    if(!QFile::exists(filePath)){
        qCWarning(playerLog) << "音频文件不存在:" << filePath;
        return false;
    }

    m_player->setSource(QUrl::fromLocalFile(filePath));
    return true;
}

void PlayerController::play(bool autoPlay)
{
    if(autoPlay) m_player->play();
}

void PlayerController::setPlayProgress(int value)
{
    m_player->setPosition(value);
}

void PlayerController::setVolume(int value)
{
    m_audioOutput->setVolume(value / 100.0);
}

QMediaPlayer *PlayerController::mediaPlayer()
{
    return m_player;
}

QAudioOutput *PlayerController::audioOutput()
{
    return m_audioOutput;
}

int PlayerController::volume()
{
    return static_cast<int>(m_audioOutput->volume() * 100);
}

int PlayerController::position()
{
    return m_player->position();
}

void PlayerController::playMusic(const QModelIndex &index, bool autoPlay)
{
    if(!this->setSource(index)){ // 设置播放源
        emit playbackError("无法播放：音频文件不在！");
        return;
    }

    this->play(autoPlay); // 播放音乐
}

void PlayerController::playOrPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    }
    else {
        // PausedState 和 StoppedState
        if (m_player->mediaStatus() >= QMediaPlayer::LoadedMedia) m_player->play();
        else qCInfo(playerLog) << "媒体正在加载中，请稍候...";
    }
}

void PlayerController::skipMusic(const QModelIndex &index)
{

    if(this->setSource(index)){ // 设置播放源
        this->play(true); // 自动播放音乐
    }
    else{
        qDebug(playerLog) << "设置播放源失败";
    }
}

