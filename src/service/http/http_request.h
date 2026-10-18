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

    // 执行get请求
    void get(const QString& url, std::function<void(QByteArray)> callback);

signals:
    void networkErrored(QNetworkReply::NetworkError error, const QString &errorString);

private:
    QNetworkAccessManager *m_manager;

    inline static int s_timeoutMs = 5000;
};

#endif // HTTP_REQUEST_H
