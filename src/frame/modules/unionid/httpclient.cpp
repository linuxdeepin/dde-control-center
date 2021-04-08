#include "httpclient.h"

Q_GLOBAL_STATIC(HttpClient, httpClient)

const QString REQUEST_URL = "https://uosvip-pre.uniontech.com";
//const QString REQUEST_URL = "http://10.4.10.104:9000";

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
}

HttpClient *HttpClient::instance()
{
    return httpClient;
}

QNetworkRequest HttpClient::setNetWorkRequest(const QString &requestApi, QNetworkRequest::KnownHeaders headerType, QVariant headerValue)
{
    QNetworkRequest networkRequest;
    networkRequest.setHeader(headerType, headerValue);
    networkRequest.setUrl(REQUEST_URL + requestApi);
    qInfo() << "NetWorkRequest" << requestApi;
    return networkRequest;
}

QNetworkReply* HttpClient::requestQrCode(const QString &clientId, const QString &deviceId
                                         , const int &scene, const QString &redirectUrl)
{
    QJsonObject json1;
    json1.insert("clientId", clientId);
    json1.insert("deviceId", deviceId);
    json1.insert("scene", scene);

    QJsonObject jsonRoot;
    jsonRoot.insert("codeInfoDTO",json1);
    jsonRoot.insert("redirectUrl",redirectUrl);

    QNetworkRequest requset = setNetWorkRequest("/qrcode/generating");
    return manager->post(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::reportQrCodeStatus(const QString &codeId, const int &qrCodeStatus,const QString &sessionId)
{
    QJsonObject dataJson;
    dataJson.insert("sessionId", sessionId);

    QJsonObject scannerJson;
    scannerJson.insert("qrCodeStatus", qrCodeStatus);
    scannerJson.insert("role", 2);
    scannerJson.insert("data", dataJson);

    QJsonObject jsonRoot;
    jsonRoot.insert("codeId", codeId);
    jsonRoot.insert("scanner", scannerJson);

    QNetworkRequest requset = setNetWorkRequest("/qrcode/event");
    return manager->put(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::getQrCodeStatus(const QString &codeId)
{
    QString requestApi = QString("/qrcode/status?codeId=%1&targetRole=%2").arg(codeId).arg(1);
    QNetworkRequest request = setNetWorkRequest(requestApi);
    return manager->get(request);
}

QNetworkReply *HttpClient::sendSmsCode(const QString &phoneNumber, const QString &accessToken)
{
    QJsonObject json;
    json.insert("accessToken", accessToken);
    json.insert("phoneNumber", phoneNumber);

    QNetworkRequest requset = setNetWorkRequest("/account/sms-code/sending");
    return manager->post(requset, QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::verifySmsCode(const QString &phoneNumber, const QString &smsCode, const QString &accessToken)
{
    QJsonObject json;
    json.insert("accessToken", accessToken);
    json.insert("phoneNumber", phoneNumber);
    json.insert("smsCode", smsCode);

    QNetworkRequest requset = setNetWorkRequest("/account/sms-code/verifying");
    return manager->post(requset, QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::bindAccount(const int &currentAccountType, const int &currentAccountId, const QString &currentAccountIdValue,
                                       const int &bindAccountType, const int &bindAccountId, const QString &bindAccountIdValue,
                                       const QString &attribute)
{
    QJsonObject json1;
    json1.insert("accountType", currentAccountType);
    json1.insert("id", currentAccountId);
    json1.insert("idValue", currentAccountIdValue);

    QJsonObject json2;
    json2.insert("accountType", bindAccountType);
    json2.insert("id", bindAccountId);
    json2.insert("idValue", bindAccountIdValue);
    json2.insert("attribute",attribute);

    QJsonObject jsonRoot;
    jsonRoot.insert("currentAccount", json1);
    jsonRoot.insert("bindAccount", json2);

    QNetworkRequest requset = setNetWorkRequest("/account/bindAccount");
    return manager->put(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::unbindAccount(const int &currentAccountType, const int &currentAccountId, const QString &currentAccountIdValue
                                         , const int &bindAccountType, const int &bindAccountId, const QString &bindAccountIdValue)
{
    QJsonObject json1;
    json1.insert("accountType", currentAccountType);
    json1.insert("id", currentAccountId);
    json1.insert("idValue", currentAccountIdValue);

    QJsonObject json2;
    json2.insert("accountType", bindAccountType);
    json2.insert("id", bindAccountId);
    json2.insert("idValue", bindAccountIdValue);

    QJsonObject jsonRoot;
    jsonRoot.insert("currentAccount", json1);
    jsonRoot.insert("bindAccount", json2);

    QNetworkRequest requset = setNetWorkRequest("/account/unbindAccount");
    return manager->post(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::getBindAccountInfo(const int &accountType, const int &id, const QString &idValue)
{
    QString requestApi = QString("http://10.5.11.205:9310/bind-accountInfo?accountType=%1&id=%2&idValue=%3")
            .arg(accountType)
            .arg(id)
            .arg(idValue);
    qInfo() << requestApi;
    QNetworkRequest requset(requestApi)/* = setNetWorkRequest(requestApi)*/;
    return manager->get(requset);
}

QNetworkReply *HttpClient::getAccessToken(const QString &clientId, const QString &code)
{
    QString requestApi = QString("/account/unionid/access-token?clientId=%1&code=%2")
            .arg(clientId)
            .arg(code);
    QNetworkRequest requset = setNetWorkRequest(requestApi);
    return manager->get(requset);
}

QNetworkReply *HttpClient::refreshAccessToken(const QString &clientId, const QString &refreshtoken)
{
    QString requestApi = QString("/account/unionid/access-token?clientId=%1&refreshToken=%2")
            .arg(clientId)
            .arg(refreshtoken);
    QNetworkRequest requset = setNetWorkRequest(requestApi);
    return manager->get(requset);
}

QNetworkReply *HttpClient::getUserInfo(const QString &accessToken)
{
    QNetworkRequest requset;
    QString qstrUrl = QString("http://api-dev.uniontech.com/v1/user?access_token=%1").arg(accessToken);
    requset.setUrl(QUrl(qstrUrl));
    requset.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkAccessManager *netmanager = new QNetworkAccessManager();
    return netmanager->get(requset);
}

QNetworkReply *HttpClient::getPictureFromUrl(const QString &url)
{
    QNetworkRequest requset;
    requset.setUrl(QUrl(url));
    requset.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    requset.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkAccessManager *netmanager = new QNetworkAccessManager();
    return netmanager->get(requset);
}

QByteArray HttpClient::checkReply(QNetworkReply *pReply)
{
    qInfo() << "checkReply";
    if (pReply->error() != QNetworkReply::NoError)
    {
        QString strErr = "Failed to request : " + pReply->url().toString()
                        + "\n the error code is : " + QString::number(pReply->error())
                        + "\n the error string is : " + pReply->errorString();
        qInfo() << "strErr" << strErr;
        return QByteArray();
    }

    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (200 != statusCode)
    {
        QString strReason = "The status code is not 200";
        qInfo() << "statusCode" << statusCode;
        return QByteArray();
    }

    QByteArray byteJson = pReply->readAll();
    //QString strJson(byteJson);
    if (byteJson.length() < 1000) {
        qInfo() << "byteJson" << byteJson;
    }

    return byteJson;
}

bool HttpClient::solveJson(const QString &strJson)
{
    QJsonObject jsonObj;
    if (!checkJson(strJson, jsonObj)) {
        return false;
    }

    QJsonValue jsonValueResult = jsonObj.value("code");
    if (REQURST_SUCCESS == jsonValueResult.toInt()) {
        return true;
    }

    return false;
}

bool HttpClient::checkJson(const QString &strJson, QJsonObject &jsonObj)
{
    QByteArray byteJson = strJson.toLocal8Bit();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        return false;
    }

    if (jsonDoc.isNull()) {
        return false;
    }

    if (!jsonDoc.isObject()) {
        return false;
    }

    jsonObj = jsonDoc.object();

    return true;
}
