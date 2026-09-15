#ifndef SONG_LIST_LOADER_H
#define SONG_LIST_LOADER_H

#include <QObject>
#include <QAbstractListModel>

class SongListModel;

class SongListLoader : public QObject
{
    Q_OBJECT
public:
    explicit SongListLoader(QObject *parent = nullptr);

    void loadSongs(SongListModel *model);

signals:
};

#endif // SONG_LIST_LOADER_H
