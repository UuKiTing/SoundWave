#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <functional>

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

    bool getSongMetaData();

    bool get(const QString& url, std::function<void(QByteArray)> callback);



signals:

private:
    QNetworkAccessManager *manager;

};

#endif // HTTPCLIENT_H
