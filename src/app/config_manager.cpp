#include "config_manager.h"
#include "user_state.h"
#include <QPushButton>
#include <QSettings>

ConfigManager::ConfigManager(QObject *parent)
    : QObject{parent}
{
}

void ConfigManager::saveSetting()
{
    QSettings s("Luo", "SoundWave");

    s.setValue("volume", m_userState.volume());
    s.setValue("position", m_userState.position());
    s.setValue("playMode", m_userState.playMode());
    s.setValue("playPage", m_userState.playPage());
    s.setValue("currentRow", m_userState.currentRow());
    s.setValue("playlist_id", m_userState.currentPlaylist());
}

void ConfigManager::loadSetting()
{
    QSettings s("Luo", "SoundWave");

    int volume = s.value("volume", 20).toInt();
    int position = s.value("position", 0).toInt();
    PlayMode mode = s.value("playMode", QVariant::fromValue(PlayMode::Loop)).value<PlayMode>();
    MainPage page = s.value("playPage", QVariant::fromValue(MainPage::Local)).value<MainPage>();
    int row = s.value("currentRow", -1).toInt();
    int playlist_id = s.value("playlist_id", -1).toInt();

    emit volumeChanged(volume);
    emit pageChanged(page, playlist_id);
    emit playmodeChanged(mode);
    emit currentRowChanged(row);
    emit positionChanged(position);
}

void ConfigManager::setVolume(float volume)
{
    m_userState.setVolume(volume);
}

void ConfigManager::setPlayPage(MainPage page)
{
    m_userState.setPlayPage(page);
}

void ConfigManager::setCurrentRow(int row)
{
    m_userState.setCurrentRow(row);
}

void ConfigManager::setPlayMode(PlayMode playMode)
{
    m_userState.setPlayMode(playMode);
}

void ConfigManager::setPosition(int position)
{
    m_userState.setPosition(position);
}

void ConfigManager::setCurrentPlaylist(int number)
{
    m_userState.setCurrentPlaylist(number);
}
