#ifndef SHTTPSOCKET_H
#define SHTTPSOCKET_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>

#define ERRCODE_STATUS_ISNOT_200 -1
#define GET_REQUEST_SUCCESS 0

enum REQUEST_TYPE {
    REQUEST_GET,
    REQUEST_POST,
    REQUEST_PUT
};

class SHttpSocket : public QObject
{
    Q_OBJECT
public:
    explicit SHttpSocket(QObject *parent = nullptr);

public:
    QNetworkReply* httpRequest(QString strRequest, REQUEST_TYPE type , QJsonObject json = QJsonObject());

    QString checkReply(QNetworkReply *pReply);

    void setHeaderType(QString headerType);

Q_SIGNALS:
    void toTellHttpRequestResult(int nCode, QString strReason);

private:
    QString m_strRequest;
    QString m_headerType;
};

#endif // SHTTPSOCKET_H
