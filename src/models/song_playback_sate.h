#ifndef SONG_PLAYBACK_SATE_H
#define SONG_PLAYBACK_SATE_H

#include "global.h"
#include <QObject>

class SongPlayBackSate : public QObject
{
    Q_OBJECT
public:
    explicit SongPlayBackSate(QObject *parent = nullptr);

    QModelIndex currentIndex();
    int currentRow();
    int listRows();
    Page currentPage();
    int playlistLastNumber();


    PlayMode mode();
    QModelIndex setNextIndex(bool isNext);
    void setCurrentIndex(const QModelIndex &index);
    void setMode(PlayMode mode);
    void setListRows(int rows);
    void setCurrentPage(Page page);
    void changeMode();
    void setPlaylistLastNumber(int number);

signals:


private:
    QModelIndex m_currentIndex; // 当前的代理索引
    PlayMode m_mode = PlayMode::Loop; // 播放模式
    int m_listRows = 0; // 当前播放列表的行数

    Page m_currentPage = Page::Local;

    int m_playlistLastNumber = -1;
};

#endif // SONG_PLAYBACK_SATE_H
