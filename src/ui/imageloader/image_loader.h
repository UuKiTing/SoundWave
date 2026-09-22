#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include "http_request.h"
#include "cover_cache_manager.h"
#include "coverutils.h"
#include <QSortFilterProxyModel>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QSet>
#include <QModelIndex>
#include <QSize>
#include <QNetworkAccessManager>
#include <QEventLoop>

/** @brief 图片任务 */
struct ImageTask{
    int song_id; ///< 歌曲id
    QString path; ///< 图片路径
    QVariant var; ///< 参数
    QSize size; ///< 图片大小
    int radius; ///< 圆角大小
    QString key; ///< 唯一标识

    ImageTask(int song_id, const QString& path, QVariant var, QSize size, int radius)
        : song_id(song_id)
        , path(path)
        , var(var)
        , size(size)
        , radius(radius)
        , key(CoverUtils::makeKey(song_id, size, radius)){}
};

/** @brief 图片合并任务 */
struct ImageMergedTask{
    int song_id; ///< 歌曲id
    QString path; ///< 图片路径
    QVector<ImageTask> arr; ///< 同一图片路径的任务队列
};

/** @brief 图片加载任务类 */
class ImageLoader : public QObject
{
    Q_OBJECT
public:
    ImageLoader(QObject *parent = nullptr);
    ~ImageLoader();

    void stop();

signals:
    /** @brief 图片已加载完毕信号 */
    void imageLoaded(int song_id, const QString &path, QVariant var);

public slots:
    /** @brief 初始化 */
    void init();

    /** @brief 添加任务 */
    void addTask(const ImageTask& task);

private:
    /** @brief 处理任务: 获取对应任务的 QImage */
    void process();

    /** @brief 对 QImage进行处理 */
    void finishImage(int song_id, const QString &path, const QImage &source, const QVector<ImageTask> &pendingTasks);

    QMutex m_mutex; ///< 互斥锁
    QQueue<ImageMergedTask> m_queue; ///< 任务队列
    QSet<int> m_set; ///< 任务集合(防止重复执行任务)

    CoverCacheManager &m_cache = CoverCacheManager::getInstance(); ///< 缓存

    HttpRequest *m_httpRequest; ///< http请求模块

    bool m_stop = false;
};

#endif // IMAGE_LOADER_H
