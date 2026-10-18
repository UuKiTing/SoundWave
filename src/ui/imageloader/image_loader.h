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

// 图片任务
struct ImageTask{
    int cover_id; // 图片id = 歌曲id
    QString path; // 图片路径
    QVariant var; // 参数
    QSize size; // 图片大小
    int radius; // 圆角大小
    QString key; // 唯一标识

    ImageTask(int cover_id, const QString& path, QVariant var, QSize size, int radius)
        : cover_id(cover_id)
        , path(path)
        , var(var)
        , size(size)
        , radius(radius)
        , key(CoverUtils::makeKey(cover_id, size, radius)){}
};

// 图片合并任务
struct ImageMergedTask{
    int cover_id; // 歌曲id
    QString path; // 图片路径
    QVector<ImageTask> arr; // 同一图片路径的任务队列
};

// 图片加载任务类
class ImageLoader : public QObject
{
    Q_OBJECT
public:
    ImageLoader(QObject *parent = nullptr);
    ~ImageLoader();

    void stop();

signals:
    // 图片加载完毕信号
    void imageLoaded(int song_id, const QString &path, QVariant var);

public slots:
    // 初始化
    void init();

    // 添加任务
    void addTask(const ImageTask& task);

private:
    // 处理任务: 获取对应任务的 QImage
    void process();

    // 对 QImage进行处理
    void finishImage(int song_id, const QString &path, const QImage &source, const QVector<ImageTask> &pendingTasks);

    QMutex m_mutex; // 互斥锁
    QQueue<ImageMergedTask> m_queue; // 任务队列
    QSet<int> m_set; // 任务集合(防止重复执行任务)

    CoverCacheManager &m_cache = CoverCacheManager::getInstance();

    HttpRequest *m_httpRequest;

    bool m_stop = false;
};

#endif // IMAGE_LOADER_H
