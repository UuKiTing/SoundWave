#include "song_list_model.h"

SongListModel::SongListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

QVariant SongListModel::data(const QModelIndex &index, int role) const
{
    const SongInfo &song = m_list[index.row()];

    switch(role){
    case Id: return song.id;
    case RemoteId: return song.remoteId;
    case Source: return QVariant::fromValue(song.source);
    case Title: return song.title;
    case Artist: return song.artist;
    case Duration: return song.duration;
    case FilePath: return song.filePath;
    case CoverPath: return song.coverPath;
    case LyricsPath: return song.lyricsPath;
    case IsFavorite: return song.isFavo;
    case IsPlaying: return song.isPlaying;
    }

    return {};
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

    m_list.append(song);

    endInsertRows();
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

    m_list[row].isFavo = isCollect;

    QModelIndex index = this->index(row, 0);
    emit dataChanged(index, index, {Roles::IsFavorite});

    return true;
}
