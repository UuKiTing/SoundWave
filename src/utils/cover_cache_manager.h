#ifndef COVER_CACHE_MANAGER_H
#define COVER_CACHE_MANAGER_H

#include <QCache>
#include <QPixmap>
#include <QMutex>

class CoverCacheManager
{
public:
    static CoverCacheManager& getInstance();

    QImage* get(const QString &key);

    bool insert(const QString &key, QImage *image, int cost = 1);

    bool contains(const QString& key);

private:
    CoverCacheManager();

private:
    mutable QCache<QString, QImage> m_coverCache;

    QMutex m_mutex;

    const int CACHE_LIMIT = 200;
};

#endif // COVER_CACHE_MANAGER_H
