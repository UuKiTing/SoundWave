#include "image_loader.h"
#include "global.h"
#include "url_config.h"
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QTimer>
#include <QThread>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>


ImageLoader::ImageLoader(QObject *parent)
    : QObject{parent}{

    m_manager = new QNetworkAccessManager(this);
}

ImageLoader::~ImageLoader(){
    stop();
}

QImage ImageLoader::loadImage(const QString &path)
{
    if (!path.startsWith("http://", Qt::CaseInsensitive) &&
        !path.startsWith("https://", Qt::CaseInsensitive)) {
        return QImage(path);
    }

    QUrl url(path);
    QNetworkRequest request(url);
    QNetworkReply *reply = m_manager->get(request);

    QEventLoop loop;

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);

    loop.exec();

    QImage img;
    if(reply->error() == QNetworkReply::NoError){
        QByteArray data = reply->readAll();
        img.loadFromData(data);
    }
    else{
        qWarning() << "Image Download Failed:" << path << "Reason:" << reply->errorString();
    }

    reply->deleteLater();

    return img;
}

void ImageLoader::addTask(const ImageTask& task)
{
    QMutexLocker locker(&m_mutex);

    // 如果已有缓存：直接返回
    if(m_cache.contains(task.key)){
        locker.unlock();

        QMetaObject::invokeMethod(this, [this, task](){
            emit imageLoaded(task.path, task.var);
        }, Qt::QueuedConnection);

        return;
    }


    // 如果已经在队列中：那么将var追加进去
    if(m_set.contains(task.path)){
        for(int i = 0; i < m_queue.size(); ++i){
            if(m_queue[i].path == task.path){
                m_queue[i].arr.append(task);
                break;
            }
        }

        return;
    }

    m_set.insert(task.path);

    ImageMergedTask mergeTask;
    mergeTask.path = task.path;
    mergeTask.arr = QVector{task};

    m_queue.enqueue(mergeTask);

    m_cond.wakeOne();
}

void ImageLoader::run()
{
    while(!m_stop){

        ImageMergedTask mergeTask;
        {
            QMutexLocker locker(&m_mutex);
            while(m_queue.isEmpty() && !m_stop){
                m_cond.wait(&m_mutex);
            }

            if(m_stop) break;

            mergeTask = m_queue.dequeue();

            m_set.remove(mergeTask.path);
        }

        QVector<ImageTask> pendingTasks;
        {
            QMutexLocker locker(&m_mutex);
            for(ImageTask &task : mergeTask.arr){
                if(m_cache.contains(task.key)){
                    emit imageLoaded(mergeTask.path, task.var);
                }
                else{
                    pendingTasks.append(task);
                }
            }
        }

        if(pendingTasks.isEmpty()){
            continue;
        }

        QImage source = loadImage(mergeTask.path);

        if(!source.isNull()){
            for(const ImageTask &task : pendingTasks){
                QImage image = CoverUtils::roundImage(source, task.size, task.radius);

                m_cache.insert(task.key, new QImage(image));

                emit imageLoaded(mergeTask.path, task.var);
            }
        }
    }
}

void ImageLoader::stop()
{
    {
        QMutexLocker locker(&m_mutex);
        m_stop = true;
    }
    m_cond.wakeAll();
}
