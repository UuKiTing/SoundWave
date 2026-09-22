#include "http_request.h"
#include <QThread>

HttpRequest::HttpRequest(QObject *parent)
    : QObject{parent}
{

    m_manager = new QNetworkAccessManager(this);
}

HttpRequest::~HttpRequest()
{
}


void HttpRequest::get(const QString& url, std::function<void(QByteArray)> callback)
{
    QNetworkRequest request(url);
    request.setTransferTimeout(5000);

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, reply, [reply, callback, this](){
        QByteArray data;

        if(reply->error() == QNetworkReply::NoError){
            data = reply->readAll();
        }
        else{
            emit networkErrored(reply->error(), reply->errorString());
        }
        if(callback){
            callback(data);
        }

        reply->deleteLater();
    });
}
