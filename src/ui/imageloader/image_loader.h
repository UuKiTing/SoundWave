#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "cover_cache_manager.h"
#include "coverutils.h"
#include <QSortFilterProxyModel>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QSet>
#include <QModelIndex>
#include <QSize>




struct ImageTask{
    QString path;
    QVariant var;
    QSize size;
    int radius;
    QString key;

    ImageTask(const QString& path, QVariant var, QSize size, int radius)
        : path(path), var(var), size(size), radius(radius), key(CoverUtils::makeKey(path, size, radius)){

    }
};


struct ImageMergedTask{
    QString path;
    QVector<ImageTask> arr;
};

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    ImageLoader(QObject *parent = nullptr);
    ~ImageLoader();

    void run();

    void stop();

    void addTask(const ImageTask& task);

signals:
    void imageLoaded(const QString &path, QVariant var);

private:
    QImage loadImage(const QString &path);

    QMutex m_mutex;
    QWaitCondition m_cond;
    QQueue<ImageMergedTask> m_queue;
    QSet<QString> m_set;

    bool m_stop = false;

    CoverCacheManager &m_cache = CoverCacheManager::getInstance();
};

#endif // IMAGE_LOADER_H
