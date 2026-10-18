#include "user_state.h"

void UserState::setCurrentPlaylist(int playlist_id)
{
    m_currentPlaylist = playlist_id;
}

int UserState::currentPlaylist()
{
    return m_currentPlaylist;
}

void UserState::setPlayPage(MainPage page)
{
    m_playPage = page;
}

MainPage UserState::playPage()
{
    return m_playPage;
}

void UserState::setCurrentRow(int row)
{
    m_currentRow = row;
}

int UserState::currentRow()
{
    return m_currentRow;
}

void UserState::setVolume(float volume)
{
    m_volume = volume * 100;
}

int UserState::volume()
{
    return m_volume;
}

void UserState::setPosition(int position)
{
    m_position=  position;
}

int UserState::position()
{
    return m_position;
}

void UserState::setPlayMode(PlayMode mode)
{
    m_playMode = mode;
}

PlayMode UserState::playMode()
{
    return m_playMode;
}
