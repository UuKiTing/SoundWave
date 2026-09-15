#include "song_playback_sate.h"
#include <QRandomGenerator>

SongPlayBackSate::SongPlayBackSate(QObject *parent)
    : QObject{parent}
{}

QModelIndex SongPlayBackSate::currentIndex()
{
    return m_currentIndex;
}

int SongPlayBackSate::currentRow()
{
    if(m_currentIndex.isValid()) return m_currentIndex.row();
    else return -1;
}

int SongPlayBackSate::listRows()
{
    return m_listRows;
}

Page SongPlayBackSate::currentPage()
{
    return m_currentPage;
}

int SongPlayBackSate::playlistLastNumber()
{
    return m_playlistLastNumber;
}


PlayMode SongPlayBackSate::mode()
{
    return m_mode;
}

QModelIndex SongPlayBackSate::setNextIndex(bool isNext)
{
    if(m_mode == PlayMode::Single) return m_currentIndex;

    int currentRow = this->currentRow();
    int total = m_listRows;

    if(total == 0) return m_currentIndex;

    if(m_mode == PlayMode::Loop){
        if(isNext) currentRow = (currentRow + 1) % total;
        else currentRow = (currentRow - 1 + total) % total;
    }
    else if(m_mode == PlayMode::Random){
        int row = QRandomGenerator::global()->bounded(total);
        while(row == currentRow && total > 1) row = QRandomGenerator::global()->bounded(total);
        currentRow = row;
    }

    this->setCurrentIndex(m_currentIndex.model()->index(currentRow, 0));

    return m_currentIndex;
}

void SongPlayBackSate::setCurrentIndex(const QModelIndex &index)
{
    m_currentIndex = index;
}

void SongPlayBackSate::setMode(PlayMode mode)
{
    m_mode = mode;
}

void SongPlayBackSate::setListRows(int rows)
{
    m_listRows = rows;
}

void SongPlayBackSate::setCurrentPage(Page page)
{
    m_currentPage = page;
}

void SongPlayBackSate::changeMode()
{
    PlayMode mode = static_cast<PlayMode>((m_mode + 1) % PlayMode::Count);
    this->setMode(mode);
}

void SongPlayBackSate::setPlaylistLastNumber(int number)
{
    m_playlistLastNumber = number;
}
