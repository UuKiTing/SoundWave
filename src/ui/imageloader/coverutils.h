#ifndef COVERUTILS_H
#define COVERUTILS_H

#include <QString>
#include <QSize>
#include <QPixmap>
#include <functional>
#include <QVariant>


namespace CoverUtils {

/**
 * @brief 异步加载封面图片
 * @param path 图片路径
 * @param size 图片大小
 * @param radius 圆角大小
 * @param onLoaded 加载成功后的回调哈数
 * @param onMissing 加载失败后的回调函数
 * @param var 任务参数
 */
bool loadCoverAsync(int song_id, const QString& path, QSize size, int radius,
                    std::function<void(const QPixmap&)> onLoaded,
                    std::function<void()> onMissing,
                    QVariant var = QVariant());


QImage roundImage(const QImage& source, const QSize& size, int radius);


QString makeKey(int song_id, const QSize& size, int radius);


} // namespace CoverUtils




#endif // COVERUTILS_H
