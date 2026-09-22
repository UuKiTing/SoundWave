#include "song_list_model.h"
#include "model_roles.h"

SongListModel::SongListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

QVariant SongListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= m_list.size()){
        return QVariant();
    }

    const SongInfo &song = m_list[row];

    switch(role){
    case Id: return song.id;
    case Source: return static_cast<int>(song.source);
    case Title: return song.title;
    case Artist: return song.artist;
    case Duration: return song.duration;
    case AudioPath: return song.audioPath;
    case CoverPath: return song.coverPath;
    case LyricsPath: return song.lyricsPath;
    case IsFavorite: return song.isFavorite;
    case IsPlaying: return song.isPlaying;
    case ProgressValue: return song.progressValue;
    case IsValid: return song.isValid;
    }

    return QVariant();
}

int SongListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return m_list.size();
}

void SongListModel::addSong(const SongInfo &song)
{
    int row = m_list.size();

    beginInsertRows(QModelIndex(), row, row);

    if(m_delSet.contains(row)){
        m_list[row].isValid = true;
    }
    else{
        m_list.append(song);

        if(!m_idToRows.contains(song.id)){
            m_idToRows[song.id].first = row;
            m_idToRows[song.id].second = -1;
        }
        else
            m_idToRows[song.id].second = row;
    }

    endInsertRows();
}

void SongListModel::addSongs(const QList<SongInfo> &songs)
{
    if (songs.isEmpty()) return;

    int firstRow = m_list.size();
    int lastRow = firstRow + songs.size() - 1;

    beginInsertRows(QModelIndex(), firstRow, lastRow);

    m_list.append(songs);

    endInsertRows();
}

void SongListModel::removeSong(int row)
{
    m_delSet.insert(row);
}

bool SongListModel::setPlayingStatus(int row)
{
    if(row < 0 || row >= rowCount()) return false;

    if(m_playingRow >= 0 && m_playingRow < rowCount()){
        m_list[m_playingRow].isPlaying = false;
        QModelIndex preIndex = this->index(m_playingRow, 0);
        emit dataChanged(preIndex, preIndex, {Roles::IsPlaying});
    }

    m_playingRow = row;
    m_list[row].isPlaying = true;

    QModelIndex index = this->index(row, 0);
    emit dataChanged(index, index, {Roles::IsPlaying});

    return true;
}

bool SongListModel::setFavorite(int row, bool isCollect)
{
    if(row < 0 || row >= rowCount()) return false;

    m_list[row].isFavorite = isCollect;

    QModelIndex index = this->index(row, 0);
    emit dataChanged(index, index, {Roles::IsFavorite});

    return true;
}

bool SongListModel::setProgress(int row, int progress)
{
    if(row < 0 || row >= rowCount()) return false;

    m_list[row].progressValue = progress;

    QModelIndex index = this->index(row, 0);
    emit dataChanged(index, index, {Roles::ProgressValue});

    return true;
}

bool SongListModel::setInvalid(int row)
{
    if(row < 0 || row >= rowCount()) return false;

    m_list[row].isValid = false;

    QModelIndex index = this->index(row, 0);
    emit dataChanged(index, index, {Roles::IsValid});

    return true;
}

QPair<int, int> SongListModel::value(int song_id)
{
    return m_idToRows.value(song_id);
}
