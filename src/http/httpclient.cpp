#include "httpclient.h"

HttpClient::HttpClient(QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager(this);



}

bool HttpClient::getSongMetaData()
{

}

bool HttpClient::get(const QString& url, std::function<void(QByteArray)> callback)
{
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, callback](){
        if(reply->error() == QNetworkReply::NoError){
            callback(reply->readAll());
        }
        reply->deleteLater();
    });
}
