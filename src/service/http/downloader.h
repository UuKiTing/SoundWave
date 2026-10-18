#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "song_repository.h"
#include <QObject>
#include <QModelIndex>
#include <QPromise>

struct SongDownloadTask{
    SongInfo song;   // 歌曲元数据
    QString audioUrl; // 远端音频地址
    QString coverUrl; // 远端封面地址
    QString lyricsUrl; // 远端歌词地址
};

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(QObject *parent = nullptr);

    // 下载歌曲数据
    void downloadSong(QModelIndex index);

    // 下载歌曲任务函数
    void downloadSongTask(QPromise<int> &promise, const SongDownloadTask &task);

    // 下载资源
    void downloadResource(const QString &url, const QString &filePath);

signals:
    //进度值更改信号
    void downloadProgressChanged(const QModelIndex &index, int progressValue);

    //添加歌曲信号
    void songAdded(SongInfo song);

private:
    SongRepository m_songRepo;

    inline static int s_timeoutMs = 5000;
};

#endif // DOWNLOADER_H
