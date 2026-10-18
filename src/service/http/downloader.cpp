#include "downloader.h"
#include "http_request.h"
#include "song_info.h"
#include "model_roles.h"
#include "path_manager.h"
#include <QFuture>
#include <QtConcurrent>

Downloader::Downloader(QObject *parent): QObject(parent) {}

void Downloader::downloadSong(QModelIndex index)
{
    if(!index.isValid()) return;

    SongDownloadTask task;

    task.song.id = index.data(Roles::Id).toInt();
    task.song.source = SongSource::Local;
    task.song.title = index.data(Roles::Title).toString();
    task.song.artist = index.data(Roles::Artist).toString();
    task.song.duration = index.data(Roles::Duration).toInt();
    task.song.durationString = toDurationString(task.song.duration);
    task.audioUrl = index.data(Roles::AudioPath).toString();
    task.coverUrl = index.data(Roles::CoverPath).toString();
    task.lyricsUrl = index.data(Roles::LyricsPath).toString();

    QFuture<int> future = QtConcurrent::run(&Downloader::downloadSongTask, this, task);

    auto *watcher = new QFutureWatcher<int>(this);

    connect(watcher, &QFutureWatcher<int>::finished, watcher, &QObject::deleteLater);
    connect(watcher, &QFutureWatcher<int>::finished, this, [this, index](){
        QTimer::singleShot(200, this, [this, index](){
            emit downloadProgressChanged(index, -1);
        });
    });

    connect(watcher, &QFutureWatcher<int>::resultReadyAt, this, [this, watcher, index](int resultIndex){
        int progressValue = watcher->resultAt(resultIndex);
        emit downloadProgressChanged(index, progressValue);
    });

    watcher->setFuture(future);
}

void Downloader::downloadSongTask(QPromise<int> &promise, const SongDownloadTask &task)
{
    QString audioFileName = task.audioUrl.split("/").back();
    QString coverFileName = task.coverUrl.split("/").back();
    QString lyricsFileName = task.lyricsUrl.split("/").back();

    promise.setProgressRange(0, 3);

    downloadResource(task.audioUrl, Paths::songAudioDir(audioFileName));
    promise.addResult(1);

    downloadResource(task.coverUrl, Paths::songImagesDir(coverFileName));
    promise.addResult(2);

    downloadResource(task.lyricsUrl, Paths::songLyricsDir(lyricsFileName));
    promise.addResult(3);

    qDebug() << "全部下载完毕";

    QList<int> collectList = m_songRepo.queryCollectSongs();

    SongInfo song;

    song = task.song;
    song.audioPath = Paths::songAudioDir(audioFileName);
    song.coverPath = Paths::songImagesDir(coverFileName);
    song.lyricsPath = Paths::songLyricsDir(lyricsFileName);
    song.isFavorite = collectList.contains(song.id);

    if(!m_songRepo.appendSong(song)){
        qDebug() << "添加歌曲失败!";
        return;
    };

    QMetaObject::invokeMethod(this, [this, song](){
        emit songAdded(song);
    });
}

void Downloader::downloadResource(const QString &url, const QString &filePath)
{
    if(url.isEmpty()) return;

    QEventLoop loop;
    HttpRequest req;

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(&req, &HttpRequest::networkErrored, &loop, &QEventLoop::quit);

    timeoutTimer.start(s_timeoutMs);

    req.get(url, [&](QByteArray data){

        if(!QFile::exists(filePath)){
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
            }
            else{
                qWarning() << "打开" << filePath << "文件失败!";
            }
        }

        loop.quit();
    });

    loop.exec();
    timeoutTimer.stop();
}
