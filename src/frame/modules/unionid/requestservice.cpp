#include "requestservice.h"

Q_GLOBAL_STATIC(RequestService, service)
const QString REQUEST_URL = "https://uosvip-pre.uniontech.com";
RequestService::RequestService(QObject *parent) : QObject(parent)
{

}

RequestService *RequestService::instance()
{
    return service;
}

QNetworkReply* RequestService::requestQrCode(const QString clientId, const QString deviceId, const int scene,  const QString redirectUrl)
{
    QJsonObject json1;
    json1.insert("clientId", clientId);
    json1.insert("deviceId", deviceId);
    json1.insert("scene", scene);

    QJsonObject jsonRoot;
    jsonRoot.insert("codeInfoDTO",json1);
    jsonRoot.insert("redirectUrl",redirectUrl);

    QString strRequest = REQUEST_URL + "/qrcode/generating";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,jsonRoot);
}

QNetworkReply* RequestService::reportQrCodeStatus(const QString codeId, const int qrCodeStatus)
{
    QJsonObject json1;
    json1.insert("qrCodeStatus", qrCodeStatus);
    json1.insert("role", 2);

    QJsonObject jsonRoot;
    jsonRoot.insert("codeId", codeId);
    jsonRoot.insert("scanner", json1);

    QString strRequest = REQUEST_URL + "/qrcode/event";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_PUT,jsonRoot);
}

QNetworkReply* RequestService::getQrCode(const QString codeId)
{
    QString strRequest = QString(REQUEST_URL + "/qrcode/info?codeId=%1").arg(codeId);
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_GET);
}

QNetworkReply* RequestService::getQrCodeStatus(const QString codeId)
{
    QString strRequest = QString(REQUEST_URL + "/qrcode/status?codeId=%1&targetRole=%2")
            .arg(codeId)
            .arg(1);

    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_GET);
}

QNetworkReply* RequestService::login(const QString type, const QString redirectUrl, const QString loginType
                           , const QString account, const QString password)
{
    QJsonObject json1;
    json1.insert("account", account);
    json1.insert("password", password);

    QJsonObject jsonRoot;
    jsonRoot.insert("type", type);
    jsonRoot.insert("redirectUrl", redirectUrl);
    jsonRoot.insert("loginType", loginType);
    jsonRoot.insert("loginData", json1);

    QString strRequest = REQUEST_URL + "/account/unionid/login";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,jsonRoot);
}

QNetworkReply* RequestService::createAccount(const QString nickname, const QString password, const QString phone)
{
    QJsonObject jsonRoot;
    jsonRoot.insert("nickname", nickname);
    jsonRoot.insert("password", password);
    jsonRoot.insert("phone", phone);

    QString strRequest = REQUEST_URL + "/account/uosunionid";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,jsonRoot);
}

QNetworkReply* RequestService::coed2Session(QString accountType, QString code)
{
    QJsonObject json;
    json.insert("accountType", accountType);
    json.insert("code", code);

    QString strRequest = REQUEST_URL + "/account/login/coed2Session";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,json);
}

QNetworkReply* RequestService::getBoundAccount(QString accountType, const QString id, const QString idValue)
{
    QJsonObject json;
    json.insert("accountType", accountType);
    json.insert("id", id);
    json.insert("idValue", 2);
    json.insert("targetAccountType",0);

    QJsonObject jsonRoot;
    jsonRoot.insert("currentAccout",json);

    QString strRequest = QString(REQUEST_URL + "/account/bound-account");

    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,jsonRoot);
}

QNetworkReply* RequestService::sendSmsCode(const QString &phoneNumber, const QString &accessToken)
{
    QJsonObject json;
    json.insert("accessToken", accessToken);
    json.insert("phoneNumber", phoneNumber);

    QString strRequest = REQUEST_URL + "/account/sms-code/sending";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,json);
}

QNetworkReply* RequestService::verifySmsCode(const QString &phoneNumber, const QString &smsCode, const QString &accessToken)
{
    QJsonObject json;
    json.insert("accessToken", accessToken);
    json.insert("phoneNumber", phoneNumber);
    json.insert("smsCode", smsCode);

    QString strRequest = REQUEST_URL + "/account/sms-code/verifying";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,json);
}

QNetworkReply* RequestService::bindAccount(const int currentAccountType, const int currentAccountId, const QString currentAccountIdValue
                                 , const int bindAccountType, const int bindAccountId, const QString bindAccountIdValue)
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

    QString strRequest = REQUEST_URL + "/account/bindAccount";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_PUT,jsonRoot);
}

QNetworkReply* RequestService::unbindAccount(const int currentAccountType, const int currentAccountId, const QString currentAccountIdValue
                                   , const int bindAccountType, const int bindAccountId, const QString bindAccountIdValue)
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

    QString strRequest = REQUEST_URL + "/account/unbindAccount";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,jsonRoot);
}

QNetworkReply* RequestService::redirectAuthInfo(const QString code, const QString state, const QString app)
{
    QJsonObject json;
    json.insert("code", code);
    json.insert("state", state);
    json.insert("app", app);

    QString strRequest = REQUEST_URL + "/account/unionid/callback";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,json);
}

QNetworkReply* RequestService::getAccessToken(const QString clientId, const QString code)
{
    QString strRequest = QString(REQUEST_URL + "/account/unionid/access-token?clientId=%1&code=%2")
            .arg(clientId)
            .arg(code);

    SHttpSocket httpSock;
//    httpSock.setHeaderType("application/x-www-form-urlencoded");
    return httpSock.httpRequest(strRequest,REQUEST_GET);
}

QNetworkReply *RequestService::refreshAccessToken(const QString clientId, const QString refreshtoken)
{
    QString strRequest = QString(REQUEST_URL + "/account/unionid/access-token?clientId=%1&refreshtoken=%2")
            .arg(clientId)
            .arg(refreshtoken);

    SHttpSocket httpSock;
//    httpSock.setHeaderType("application/x-www-form-urlencoded");
    return httpSock.httpRequest(strRequest,REQUEST_GET);
}

QNetworkReply* RequestService::decrptData(const QString encrytedData, const QString iv, const QString sessionId)
{
    QJsonObject json;
    json.insert("encrytedData", encrytedData);
    json.insert("iv", iv);
    json.insert("sessionId", sessionId);

    QString strRequest = REQUEST_URL + "/account/decrypting";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_POST,json);
}

QNetworkReply* RequestService::queryUserPromotionResult(const QString unionId)
{
    QString strRequest = QString(REQUEST_URL + "/activation/promotion/status?unionId=%1").arg(unionId);
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_GET);
}

QNetworkReply* RequestService::reportUserPromotionResult(const QString unionId, const QString sessionId)
{
    QJsonObject json;
    json.insert("unionId", unionId);
    json.insert("sessionId", sessionId);

    QString strRequest = REQUEST_URL + "/activation/promotion/status";
    SHttpSocket httpSock;
    return httpSock.httpRequest(strRequest,REQUEST_PUT,json);
}

QNetworkReply *RequestService::getUserInfo(const QString &accessToken)
{
//    QString data = QString("access_token=%1").arg(accessToken);
    QNetworkRequest requset;
    QString qstrUrl = QString("http://api-dev.uniontech.com/v1/user?access_token=%1").arg(accessToken);
    requset.setUrl(QUrl(qstrUrl));
//    requset.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-from-urlencoded");
    requset.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkAccessManager *manager = new QNetworkAccessManager();
//    return manager->post(requset,/*data.toUtf8()*/QByteArray());
    return manager->get(requset);
}

QString RequestService::checkReply(QNetworkReply *pReply)
{
    SHttpSocket httpSock;
    return httpSock.checkReply(pReply);
}

bool RequestService::solveJson(QString strJson)
{
    QJsonObject jsonObj;
    if (!checkJson(strJson, jsonObj))
    {
        return false;
    }

    QJsonValue jsonValueResult = jsonObj.value("code");
    //if (jsonValueResult.isString())
    //{
        int nResult = jsonValueResult.toInt();

        if (nResult == REQURST_SUCCESS)
        {
            return true;
        }
    //}

        return false;
}

bool RequestService::checkJson(QString strJson, QJsonObject &jsonObj)
{
    QByteArray byteJson = strJson.toLocal8Bit();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);

    if (jsonError.error != QJsonParseError::NoError)
    {
        return false;
    }

    if (jsonDoc.isNull())
    {
        return false;
    }

    if (!jsonDoc.isObject())
    {
        return false;
    }

    jsonObj = jsonDoc.object();

    return true;
}

