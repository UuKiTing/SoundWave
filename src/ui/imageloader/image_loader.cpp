#include "image_loader.h"
#include "url_config.h"
#include "http_request.h"
#include "url_utils.h"
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QTimer>
#include <QThread>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <future>
#include <memory>
#include <QByteArray>
#include <QCoreApplication>

ImageLoader::ImageLoader(QObject *parent)
    : QObject{parent}{

}

ImageLoader::~ImageLoader(){
}

void ImageLoader::stop()
{
    m_stop = true;
}

void ImageLoader::addTask(const ImageTask& task)
{
    {
        QMutexLocker locker(&m_mutex);

        if(m_cache.contains(task.key)){

            locker.unlock();

            QMetaObject::invokeMethod(this, [this, task](){
                emit imageLoaded(task.cover_id, task.path, task.var);
            }, Qt::QueuedConnection);

            return;
        }

        if(m_set.contains(task.cover_id)){
            for(int i = 0; i < m_queue.size(); ++i){
                if(m_queue[i].cover_id == task.cover_id){
                    m_queue[i].arr.append(task);
                    locker.unlock();
                    return;
                }
            }
        }

        m_set.insert(task.cover_id);

        ImageMergedTask mergeTask;
        mergeTask.path = task.path;
        mergeTask.cover_id = task.cover_id;
        mergeTask.arr = QVector<ImageTask>{task};

        m_queue.enqueue(mergeTask);
    }

    QMetaObject::invokeMethod(this, &ImageLoader::process, Qt::QueuedConnection);
}

void ImageLoader::process()
{
    if(m_stop) return;

    ImageMergedTask mergeTask;
    {
        QMutexLocker locker(&m_mutex);

        if(m_queue.isEmpty()) return;

        mergeTask = m_queue.dequeue();

        m_set.remove(mergeTask.cover_id);
    }

    QVector<ImageTask> pendingTasks;
    {
        QMutexLocker locker(&m_mutex);
        for(ImageTask &task : mergeTask.arr){
            if(m_cache.contains(task.key)){
                emit imageLoaded(mergeTask.cover_id, mergeTask.path, task.var);
            }
            else{
                pendingTasks.append(task);
            }
        }
    }

    if(pendingTasks.isEmpty()){
        return;
    }

    QString path = mergeTask.path;
    int song_id = mergeTask.cover_id;

    if (!UrlUtils::isHttpUrl(path)) {
        QImage source(path);

        finishImage(song_id, path, source, pendingTasks);
    }
    else{
        m_httpRequest->get(path, [this, pendingTasks, path, song_id](QByteArray data) {
            QImage img;
            img.loadFromData(data);

            finishImage(song_id, path, img, pendingTasks);
        });
    }
}

void ImageLoader::finishImage(int song_id, const QString &path, const QImage &source, const QVector<ImageTask> &pendingTasks)
{
    if(source.isNull()) return;

    for(const ImageTask &task : pendingTasks){
        QImage image = CoverUtils::roundImage(source, task.size, task.radius);

        m_cache.insert(task.key, new QImage(image));

        emit imageLoaded(song_id, path, task.var);
    }

}

void ImageLoader::init()
{
    m_httpRequest = new HttpRequest(this);
}
