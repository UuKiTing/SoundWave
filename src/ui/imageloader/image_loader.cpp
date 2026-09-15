#include "image_loader.h"
#include "global.h"
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QTimer>
#include <QThread>

ImageLoader::ImageLoader(QObject *parent)
    : QObject{parent}{}

ImageLoader::~ImageLoader(){
    stop();
}

QImage ImageLoader::loadImage(const QString &path)
{
    return QImage(path);
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

        for(ImageTask &task : mergeTask.arr){
            if(m_cache.contains(task.key)){
                emit imageLoaded(mergeTask.path, task.var);
            }

            continue;
        }

        QImage source = loadImage(mergeTask.path);

        if(!source.isNull()){
            for(int i = 0; i < mergeTask.arr.size(); ++i){
                ImageTask task = mergeTask.arr[i];

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
