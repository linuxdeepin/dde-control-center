#include "httpclient.h"

#include <QFile>
#include <DApplication>
#include <DSysInfo>

Q_GLOBAL_STATIC(HttpClient, httpClient)

DCORE_USE_NAMESPACE

const QByteArray CLIENT_ID = "388340d186f311eb983b0242ac130002";
const QString REQUEST_URL = "https://uosvip.uniontech.com";
//const QString REQUEST_URL = "http://10.4.10.104:9000";

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
    loadDeepinDev();
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
    networkRequest.setRawHeader("client_id", CLIENT_ID);
    //    networkRequest.setRawHeader("uos_license", Utils::fileContent("/var/cache/gather/glicense.dat").toUtf8());

    if(m_strDeepinHttpName.isEmpty()){
        networkRequest.setUrl(REQUEST_URL + requestApi);
    }else {
        networkRequest.setUrl(m_strDeepinHttpName + requestApi);
    }

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
    qInfo() << QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact);
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
    QString requestApi = QString("/account/bind-accountInfo?accountType=%1&id=%2&idValue=%3")
            .arg(accountType)
            .arg(id)
            .arg(idValue);
    QNetworkRequest requset = setNetWorkRequest(requestApi);
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

/*******************************************************************************
 1. @函数:    loadDeepinDev
 2. @作者:    ut000610 郁佳玮
 3. @日期:    2021-04-21
 4. @说明:    获取/etc/environment下的deepindv信息
*******************************************************************************/
void HttpClient::loadDeepinDev()
{
    QString path = "/etc/environment";
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly)) {
        qInfo()<<"Can't open the file!"<<endl;
        return;
    }

    QStringList lstDeepinDev;
    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        QString strDeepinDev(line);
        lstDeepinDev.append(strDeepinDev);
    }

    if(0 == lstDeepinDev.length())
    {
        qInfo()<<"have not DEEPINID_DEV group!"<<endl;
        return;
    }

    QStringList lstDeepinDevInfo;
    QString strFileDevName = "DEEPINID_DEV";
    for(int i = 0; i < lstDeepinDev.length(); ++i)
    {
        if(lstDeepinDev[i].contains(strFileDevName))
        {
            lstDeepinDevInfo = lstDeepinDev[i].split("=");
        }
    }

    if(2 != lstDeepinDevInfo.length() || lstDeepinDevInfo[1].isEmpty() || "\n" == lstDeepinDevInfo[1])
    {
        qInfo()<<"HttpInfo is error!"<<endl;
        return;
    }

    m_strDeepinHttpName = lstDeepinDevInfo.at(1);
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
