#ifndef SONG_LIST_LOADER_H
#define SONG_LIST_LOADER_H


#include "httpclient.h"
#include "url_config.h"
#include <QObject>
#include <QAbstractListModel>


class SongListModel;

class SongListLoader : public QObject
{
    Q_OBJECT
public:
    explicit SongListLoader(QObject *parent = nullptr);

    void loadSongs(SongListModel *model);

    void loadRemoteSongs(SongListModel *model);

signals:

private:

    HttpClient *httpClient;
};

#endif // SONG_LIST_LOADER_H
