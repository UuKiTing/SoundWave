#include "player_controller.h"
#include "model_roles.h"
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

    connect(m_audioOutput, &QAudioOutput::volumeChanged, this, &PlayerController::volumeChanged);

    // 将QMediaPlayer的信号转发出去
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &PlayerController::playbackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &PlayerController::mediaStatusChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlayerController::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &PlayerController::positionChanged);
}

bool PlayerController::setSource(const QModelIndex &index)
{
    QString filePath = index.data(Roles::AudioPath).toString();

    if(filePath.isEmpty()){
        qCWarning(playerLog) << "音频路径为空!";
        return false;
    }

    QUrl url;

    // 如果是http地址
    if (filePath.startsWith("http://", Qt::CaseInsensitive) ||
        filePath.startsWith("https://", Qt::CaseInsensitive)) {

        url  = QUrl::fromUserInput(filePath);
    }
    else{ // 如果是本地路径
        if(!QFile::exists(filePath)){
            qCWarning(playerLog) << "音频文件不存在:" << filePath;
            return false;
        }

        url = QUrl::fromLocalFile(filePath);
    }

    m_player->setSource(url);

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

int PlayerController::playProgress()
{
    return m_player->position();
}

void PlayerController::setVolume(int value)
{
    m_audioOutput->setVolume(value / 100.0);
}

int PlayerController::volume()
{
    return static_cast<int>(m_audioOutput->volume() * 100);
}

void PlayerController::playSong(const QModelIndex &index, bool autoPlay)
{
    if(!this->setSource(index)){
        emit playbackError("无法播放：音频文件不在！");
        return;
    }

    this->play(autoPlay);
}

void PlayerController::playOrPause()
{
    // 如果处于播放状态则暂停
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } // 如果处于暂停或者停止状态则播放
    else if(m_player->playbackState() == QMediaPlayer::PausedState ||
            m_player->playbackState() == QMediaPlayer::StoppedState){
        m_player->play();
    }
}

void PlayerController::skipSong(const QModelIndex &index)
{

    if(this->setSource(index)){
        this->play(true);
    }
    else{
        qDebug(playerLog) << "设置播放源失败";
    }
}

