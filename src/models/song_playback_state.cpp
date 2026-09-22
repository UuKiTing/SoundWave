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

void SongPlaybackState::setPlaylistPlayingNumber(int number)
{
    m_playlistPlayingNumber = number;
}

int SongPlaybackState::playlistPlayingNumber()
{
    return m_playlistPlayingNumber;
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
    if(m_playMode == PlayMode::Single) return m_currentRow;

    int currentRow = this->currentRow();
    int total = m_listRows;

    if(total == 0) return m_currentRow;

    if(m_playMode == PlayMode::Loop){
        if(isNext) currentRow = (currentRow + 1) % total;
        else currentRow = (currentRow - 1 + total) % total;
    }
    else if(m_playMode == PlayMode::Random){
        int row = QRandomGenerator::global()->bounded(total);
        while(row == currentRow && total > 1) row = QRandomGenerator::global()->bounded(total);
        currentRow = row;
    }

    this->setCurrentRow(currentRow);

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
