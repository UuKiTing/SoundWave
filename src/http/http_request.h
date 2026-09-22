#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <functional>

class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QObject *parent = nullptr);
    ~HttpRequest();

    /**
     * @brief 执行get请求
     * @note 封装get请求的标准流程
     * @param url 网络地址
     * @param callback 回调函数
     */
    void get(const QString& url, std::function<void(QByteArray)> callback);

signals:
    void networkErrored(QNetworkReply::NetworkError error, const QString &errorString);

private:
    QNetworkAccessManager *m_manager; ///< 网络访问管理器
};

#endif // HTTP_REQUEST_H
