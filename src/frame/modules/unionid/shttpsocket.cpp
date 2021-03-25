#include "shttpsocket.h"
#include <QJsonDocument>
#include <QJsonObject>

SHttpSocket::SHttpSocket(QObject *parent) : QObject(parent)
{
    m_headerType = "application/json";
}

QNetworkReply* SHttpSocket::httpRequest(QString strRequest, REQUEST_TYPE type, QJsonObject json)
{
    m_strRequest = strRequest;

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest network_request;

    network_request.setHeader(QNetworkRequest::ContentTypeHeader, m_headerType);
    network_request.setUrl(strRequest);
    m_headerType = "application/json";
    qInfo() << "network_request_URL" << QUrl(strRequest);

    if (!json.isEmpty()) {
        qInfo() << "sendJson" << QJsonDocument(json).toJson(QJsonDocument::Compact);
    }

    QNetworkReply *pReply = nullptr;

    switch (type) {
    case REQUEST_GET:
        pReply = manager->get(network_request);
        break;
    case REQUEST_POST:
        pReply = manager->post(network_request,QJsonDocument(json).toJson(QJsonDocument::Compact));
        break;
    case REQUEST_PUT:
        pReply = manager->put(network_request,QJsonDocument(json).toJson(QJsonDocument::Compact));
        break;
    }

    return pReply;
//    QEventLoop eventLoop;
//    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
//    eventLoop.exec();

//    if (pReply->error() != QNetworkReply::NoError)
//    {
//        int nErrCode = (int)(pReply->error());
//        QString strReason = pReply->errorString();
//        QString strErr = "Failed to request : "
//                + m_strRequest
//                + ", the error code is : "
//                + QString::number(nErrCode)
//                + ", the error string is : "
//                + strReason;

//        qInfo() << "strErr" << strErr;
//        toTellHttpRequestResult(nErrCode, strReason);

//        return;
//    }

//    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//    if (statusCode != 200)
//    {
//        int nErrCode = ERRCODE_STATUS_ISNOT_200;
//        QString strReason = "The status code is not 200";

//        toTellHttpRequestResult(nErrCode, strReason);

//        return;
//    }

//    QByteArray byteJson = pReply->readAll();
//    QString strJson(byteJson);
//    qInfo() << "strJson" << strJson;

    //    toTellHttpRequestResult(GET_REQUEST_SUCCESS, strJson);
}

QString SHttpSocket::checkReply(QNetworkReply *pReply)
{
    if (pReply->error() != QNetworkReply::NoError)
    {
        int nErrCode = (int)(pReply->error());
        QString strReason = pReply->errorString();
        QString strErr = "Failed to request : "
                + m_strRequest
                + ", the error code is : "
                + QString::number(nErrCode)
                + ", the error string is : "
                + strReason;

        qInfo() << "strErr" << strErr << pReply->url();
        toTellHttpRequestResult(nErrCode, strReason);

        return "";
    }

    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200)
    {
        int nErrCode = ERRCODE_STATUS_ISNOT_200;
        QString strReason = "The status code is not 200";

        toTellHttpRequestResult(nErrCode, strReason);

        return "";
    }

    QByteArray byteJson = pReply->readAll();
    QString strJson(byteJson);
    qInfo() << "strJson" << strJson;
    return strJson;
}

void SHttpSocket::setHeaderType(QString headerType)
{
    m_headerType = headerType;
}

