#ifndef URL_UTILS_H
#define URL_UTILS_H

#include <QString>

namespace UrlUtils {

// 判断给定路径是否为 http/https 远程地址
inline bool isHttpUrl(const QString &path)
{
    return path.startsWith("http://", Qt::CaseInsensitive) ||
           path.startsWith("https://", Qt::CaseInsensitive);
}

} // namespace UrlUtils

#endif // URL_UTILS_H
