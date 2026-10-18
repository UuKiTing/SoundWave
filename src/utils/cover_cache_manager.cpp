#include "cover_cache_manager.h"

CoverCacheManager::CoverCacheManager() {
    m_coverCache.setMaxCost(CACHE_LIMIT);
}

CoverCacheManager &CoverCacheManager::getInstance()
{
    static CoverCacheManager manager;
    return manager;
}

QImage *CoverCacheManager::get(const QString &key) const
{
    QReadLocker locker(&m_lock);
    return m_coverCache.object(key);
}

bool CoverCacheManager::insert(const QString &key, QImage *image, int cost)
{
    QWriteLocker locker(&m_lock);
    return m_coverCache.insert(key, image, cost);
}

bool CoverCacheManager::contains(const QString &key)
{
    QReadLocker locker(&m_lock);
    return m_coverCache.contains(key);
}
