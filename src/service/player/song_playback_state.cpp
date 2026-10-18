#include "song_playback_state.h"
#include <QRandomGenerator>

SongPlaybackState::SongPlaybackState(QObject *parent)
    : QObject{parent}
{}

void SongPlaybackState::setListRows(int rows)
{
    m_listRows = rows;
}

int SongPlaybackState::listRows()
{
    return m_listRows;
}

void SongPlaybackState::setCurrentPlaylist(int playlist_id)
{
    m_currentPlaylist = playlist_id;
}

int SongPlaybackState::currentPlaylist()
{
    return m_currentPlaylist;
}

void SongPlaybackState::changePlayMode()
{
    PlayMode mode = static_cast<PlayMode>((m_playMode + 1) % PlayMode::_Count);
    this->setPlayMode(mode);
}

PlayMode SongPlaybackState::playMode()
{
    return m_playMode;
}

void SongPlaybackState::setPlayMode(PlayMode mode)
{
    m_playMode = mode;
}

void SongPlaybackState::setCurrentPage(MainPage page)
{
    m_currentPage = page;
}

MainPage SongPlaybackState::currentPage()
{
    return m_currentPage;
}

int SongPlaybackState::setNextRow(bool isNext)
{
    const int total = m_listRows;
    if(total <= 0) return m_currentRow;

    int nextRow = m_currentRow;

    switch(m_playMode){
    case PlayMode::Loop:
        nextRow = calculateLoopRow(m_currentRow, isNext);
        break;

    case PlayMode::Single:
        return m_currentRow;

    case PlayMode::Random:
        nextRow = calculateRandRow(m_currentRow);
        break;
    }

    setCurrentRow(nextRow);

    return m_currentRow;
}

void SongPlaybackState::setCurrentRow(int row)
{
    m_currentRow = row;
}

int SongPlaybackState::currentRow()
{
    return m_currentRow;
}

void SongPlaybackState::setProxyId(ProxyId id)
{
    m_proxyId = id;
}

ProxyId SongPlaybackState::proxyId()
{
    return m_proxyId;
}

int SongPlaybackState::calculateLoopRow(int row, bool isNext)
{
    const int offset = isNext ? 1 : -1;
    return (row + offset + m_listRows) % m_listRows;
}

int SongPlaybackState::calculateRandRow(int currentRow)
{
    if(m_listRows <= 1) return currentRow;

    int row;
    do{
        row = QRandomGenerator::global()->bounded(m_listRows);
    }
    while(row == currentRow);

    return row;
}
