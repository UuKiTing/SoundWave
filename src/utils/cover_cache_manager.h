#ifndef COVER_CACHE_MANAGER_H
#define COVER_CACHE_MANAGER_H

#include <QCache>
#include <QPixmap>
#include <QMutex>

/**
 * @brief 封面图片缓存管理
 */
class CoverCacheManager
{
public:
    static CoverCacheManager& getInstance();

    /** @brief 根据key获取对应的QImage */
    QImage* get(const QString &key);

    /** @brief 插入记录到缓存中 */
    bool insert(const QString &key, QImage *image, int cost = 1);

    /** @brief 判断key是否在缓存中 */
    bool contains(const QString& key);

private:
    CoverCacheManager();

private:
    mutable QCache<QString, QImage> m_coverCache;

    QMutex m_mutex; ///< 互斥锁

    const int CACHE_LIMIT = 200; ///< 缓存最大限制
};

#endif // COVER_CACHE_MANAGER_H
