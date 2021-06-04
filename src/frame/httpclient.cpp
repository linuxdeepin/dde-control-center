#include "httpclient.h"

#include <QFile>
#include <DApplication>
#include <DSysInfo>

Q_GLOBAL_STATIC(HttpClient, httpClient)

DCORE_USE_NAMESPACE

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
    m_redirec_url = "https://uosvip.uniontech.com/account/unionid/callback/uid-management";
    m_request_url = "https://uosvip.uniontech.com";
    m_clientid = "fc8b4f1c34644fd184e002ecdcc6a295";
    judgeClienid();
}

HttpClient *HttpClient::instance()
{
    return httpClient;
}

QNetworkRequest HttpClient::setNetWorkRequest(const QString &requestApi, QNetworkRequest::KnownHeaders headerType, QVariant headerValue)
{
    QNetworkRequest networkRequest;
    networkRequest.setHeader(headerType, headerValue);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, userAgentInfo());
    networkRequest.setRawHeader("client_id", m_clientid.toUtf8());
    //    networkRequest.setRawHeader("uos_license", Utils::fileContent("/var/cache/gather/glicense.dat").toUtf8());
    networkRequest.setUrl(m_request_url + requestApi);

    return networkRequest;
}

QNetworkReply *HttpClient::requestQrCode(const QString &clientId, const QString &deviceId
                                         , const int &scene, const QString &redirectUrl)
{
    QJsonObject json1;
    json1.insert("clientId", clientId);
    json1.insert("deviceId", deviceId);
    json1.insert("scene", scene);

    QJsonObject jsonRoot;
    jsonRoot.insert("codeInfoDTO", json1);
    jsonRoot.insert("redirectUrl", redirectUrl);

    QNetworkRequest requset = setNetWorkRequest("/qrcode/generating");
    return httpRequset("post",requset,QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
//    return manager->post(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::reportQrCodeStatus(const QString &codeId, const int &qrCodeStatus, const QString &sessionId)
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
    return httpRequset("put",requset,QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
//    return manager->put(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::getQrCodeStatus(const QString &codeId)
{
    QString requestApi = QString("/qrcode/status?codeId=%1&targetRole=%2").arg(codeId).arg(1);
    QNetworkRequest request = setNetWorkRequest(requestApi);
    return httpRequset("get",request,"");
//    return manager->get(request);
}

QNetworkReply *HttpClient::sendSmsCode(const QString &phoneNumber, const QString &accessToken)
{
    QJsonObject json;
    json.insert("accessToken", accessToken);
    json.insert("phoneNumber", phoneNumber);

    QNetworkRequest requset = setNetWorkRequest("/account/sms-code/sending");
    return httpRequset("post",requset,QJsonDocument(json).toJson(QJsonDocument::Compact));
//    return manager->post(requset, QJsonDocument(json).toJson(QJsonDocument::Compact));
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
    json2.insert("attribute", attribute);

    QJsonObject jsonRoot;
    jsonRoot.insert("currentAccount", json1);
    jsonRoot.insert("bindAccount", json2);

    QNetworkRequest requset = setNetWorkRequest("/account/bindAccount");
    return httpRequset("put",requset,QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
//    return manager->put(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
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
    return httpRequset("post",requset,QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
//    return manager->post(requset, QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact));
}

QNetworkReply *HttpClient::getBindAccountInfo(const int &accountType, const int &id, const QString &idValue)
{
    QString requestApi = QString("/account/bind-accountInfo?accountType=%1&id=%2&idValue=%3")
                         .arg(accountType)
                         .arg(id)
                         .arg(idValue);
    QNetworkRequest requset = setNetWorkRequest(requestApi);
    return httpRequset("get",requset,"");
//    return manager->get(requset);
}

QNetworkReply *HttpClient::getAccessToken(const QString &clientId, const QString &code)
{
    QString requestApi = QString("/account/unionid/access-token?clientId=%1&code=%2")
                         .arg(clientId)
                         .arg(code);
    QNetworkRequest requset = setNetWorkRequest(requestApi);

    return httpRequset("get",requset,"");
//    return manager->get(requset);
}

QNetworkReply *HttpClient::refreshAccessToken(const QString &clientId, const QString &refreshtoken)
{
    QString requestApi = QString("/account/unionid/access-token?clientId=%1&refreshToken=%2")
                         .arg(clientId)
                         .arg(refreshtoken);
    QNetworkRequest requset = setNetWorkRequest(requestApi);

    return httpRequset("get",requset,"");
//    return manager->get(requset);
}

QNetworkReply *HttpClient::getUserInfo(const QString &accessToken)
{
    QNetworkRequest requset;
    QString qstrUrl = QString("https://api.uniontech.com/v1/user?access_token=%1").arg(accessToken);
    requset.setUrl(QUrl(qstrUrl));
    requset.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//    QNetworkAccessManager *netmanager = new QNetworkAccessManager();
    return httpRequset("get",requset,"");
//    return netmanager->get(requset);
}

QNetworkReply *HttpClient::getPictureFromUrl(const QString &url)
{
    QNetworkRequest requset;
    requset.setUrl(QUrl(url));
    requset.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    requset.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//    QNetworkAccessManager *netmanager = new QNetworkAccessManager();
    return httpRequset("get",requset,"");
    //return netmanager->get(requset);
}

QByteArray HttpClient::checkReply(QNetworkReply *pReply)
{
    QByteArray byteJson = "";

    if (pReply->error() != QNetworkReply::NoError) {
        QString strErr = "Failed to request : " + pReply->url().toString()
                         + "\n the error code is : " + QString::number(pReply->error())
                         + "\n the error string is : " + pReply->errorString();
        qInfo() << "strErr" << strErr;
        //return QByteArray();
    }

    int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (200 != statusCode) {
        QString strReason = "The status code is not 200";
        qInfo() << "statusCode" << statusCode;
        //return QByteArray();
    }

    byteJson = pReply->readAll();
    //QString strJson(byteJson);
    if (byteJson.length() < 1000) {
        //qInfo() << "byteJson" << byteJson;
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

QString HttpClient::getClientId()
{
    return m_clientid;
}

QString HttpClient::getRedirecUrl()
{
    return m_redirec_url;
}

QString HttpClient::getRequestUrl()
{
    return m_request_url;
}

void HttpClient::onGetAccessToken(const QString &code)
{
    QNetworkReply *reply = getAccessToken(m_clientid,code);
    connect(reply,&QNetworkReply::finished, this, &HttpClient::onGetAccessTokenFinished);
}

void HttpClient::onGetPictureFromUrl(const QString& avatarUrl)
{
    QNetworkReply *reply = getPictureFromUrl(avatarUrl);
    connect(reply,&QNetworkReply::finished, this, &HttpClient::onGetPictureFromUrlFinished);
}

void HttpClient::onBindAccountInfo(const int &accountType, const int& id, const QString& idValue)
{
    QNetworkReply *reply = getBindAccountInfo(accountType,id,idValue);
    connect(reply,&QNetworkReply::finished, this, &HttpClient::onBindAccountInfoFinished);
}

void HttpClient::onRefreshAccessToken(const QString &clientId, const QString &refreshtoken)
{
    QNetworkReply *reply = refreshAccessToken(clientId,refreshtoken);
    connect(reply,&QNetworkReply::finished, this, &HttpClient::onRefreshAccessTokenFinished);
}

void HttpClient::onGetAccessTokenFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    Q_EMIT toTellGetAccessTokenFinished(checkReply(reply));;
    reply->deleteLater();
}

void HttpClient::onGetPictureFromUrlFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    Q_EMIT toTellGetPictureFromUrlFinished(checkReply(reply));;
    reply->deleteLater();
}

void HttpClient::onBindAccountInfoFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    Q_EMIT toTellBindAccountInfoFinished(checkReply(reply));;
    reply->deleteLater();
}

void HttpClient::onRefreshAccessTokenFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    Q_EMIT toTellRefreshAccessTokenFinished(checkReply(reply));;
    reply->deleteLater();
}

/*******************************************************************************
<<<<<<< HEAD
 1. @函数:    judgeClienid
=======
 1. @函数:    loadDeepinDev
>>>>>>> ae2e708e367479c07635a9054dd1df997814ca50
 2. @作者:    ut003285 刘冬宇
 3. @日期:    2021-04-23
 4. @说明:    判断/etc/environment下的DEEPIN_DEV中是否存在-pre字段
*******************************************************************************/
void HttpClient::judgeClienid()
{
    QString path = "/etc/environment";
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        qInfo() << "Can't open /etc/environment file!" << endl;
        return;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (QString(line).contains("DEEPIN_DEV=")) {
            QStringList temp = QString(line).split("=");
            if (temp.at(0) == "DEEPIN_DEV" && !temp.at(1).isEmpty()) {
                if (temp.at(1).contains("-pre")) {
                    m_redirec_url = "https://uosvip-pre.uniontech.com/account/unionid/callback/uid-managment";
                    m_request_url = "https://uosvip-pre.uniontech.com";
                    m_clientid = "388340d186f311eb983b0242ac130002";
                } else {
                    m_redirec_url = "https://uosvip.uniontech.com/account/unionid/callback/uid-management";
                    m_request_url = "https://uosvip.uniontech.com";
                    m_clientid = "fc8b4f1c34644fd184e002ecdcc6a295";
                }
                break;
            }
        }
    }

    file.close();

}

QNetworkReply* HttpClient::httpRequset(const QString &type, const QNetworkRequest &requset, const QByteArray &body)
{
    //qInfo() << type << requset.url() << body;
    if (type == "get") {
        return manager->get(requset);
    } else if (type == "post") {
        return manager->post(requset,body);
    } else if (type == "put") {
        return manager->put(requset,body);
    } else {
        return nullptr;
    }
}

/*******************************************************************************
 1. @函数:    userAgentInfo
 2. @作者:    ut000610 戴正文
 3. @日期:    2021-04-15
 4. @说明:    获取UserAgent信息
 1）device_id /etc/machine-id 32位
 2）client                    应用名
 3) client_version            应用版本
 4）os-version                操作系统版本
 5）channel                   暂无此参数
*******************************************************************************/
QString HttpClient::userAgentInfo()
{
    QString deviceId = fileContent("/etc/machine-id");
    QString client = qAppName();
    QString clientVersion = "";
    QString os = DSysInfo::uosEditionName(QLocale(QLocale::English));
    QString osVersion = DSysInfo::minorVersion();

    QString strUserAgent;
    strUserAgent = QString("deviceId/%1 ").arg(deviceId)
                   + QString("client/%1 ").arg(client)
                   + QString("client_version/%1 ").arg(clientVersion)
                   + QString("os/%1 ").arg(os)
                   + QString("os_version/%1 ").arg(osVersion)
                   + QString("channel/");
    qDebug() << strUserAgent;

    return strUserAgent;
}

/*******************************************************************************
 1. @函数:    deviceId
 2. @作者:    ut000610 戴正文
 3. @日期:    2021-04-15
 4. @说明:    读取文件获取文件内容
*******************************************************************************/
QString HttpClient::fileContent(const QString &fileName)
{
    QString fileData;
    QFile file(fileName);

    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            // 读取文件内容
            fileData = file.readAll();
        }
    }

    file.close();

    qDebug() << fileName << fileData;
    return fileData.trimmed();
}
