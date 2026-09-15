#ifndef SONG_LIST_MODEL_H
#define SONG_LIST_MODEL_H

#include "global.h"
#include <QAbstractListModel>

class SongListModel : public QAbstractListModel
{
    Q_OBJECT
public:


    explicit SongListModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void addSong(const SongInfo& song);

    bool setPlayingStatus(int row);

    bool setFavorite(int row, bool isCollect);

private:
    QList<SongInfo> m_list;

    int m_playingRow = -1;
};

#endif // SONG_LIST_MODEL_H
