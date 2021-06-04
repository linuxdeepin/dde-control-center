#ifndef REQUESTSERVICE_H
#define REQUESTSERVICE_H

#include <QObject>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QNetworkReply>

enum REQURST_RESULT {
    REQURST_SUCCESS,
    REQURST_FAIL
};

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

    static HttpClient *instance();
    //扫码服务
    /**
     * @brief requestQrCode 请求生成二维码
     * @param clientId      端侧ID
     * @param deviceId      设备唯一ID
     * @param scene         二维码类型     1.注册UOS账号
                                         2.登录UOS账号
                                         3.UOS账号绑定微信号
                                         4.创建本地账号
                                         5.登录众包平台
                                         6.登录主机 手机电脑协同连接
                                         7.激活UOS系统
     * @param redirectUrl   开放平台回调地址
     */
    QNetworkReply* requestQrCode(const QString &clientId, const QString &deviceId, const int &scene, const QString &redirectUrl);

    /**
     * @brief reportQrCodeStatus  上报二维码状态
     * @param codeId              UUID生成唯一序列
     * @param qrCodeStatus        扫码状态           1.已扫码
                                                    2.扫码成功-已绑定
                                                    3.扫码成功-未绑定
                                                    4.已完成
     * @param role                扫码标志           1：消费端（手机扫码）
                                                    2：生产端（PC）
     * @param entityId            实体对象值         电脑端传null 手机端必填
     * @param sessionId
     */
    QNetworkReply *reportQrCodeStatus(const QString &codeId, const int &qrCodeStatus,const QString &sessionId);

    /**
     * @brief getQrCodeStatus  查询扫码状态
     * @param codeId           UUID生成唯一序列
     * @param targetRole       扫码标志           1：消费端（手机扫码）
                                                 2：生产端（PC）
     */
    QNetworkReply *getQrCodeStatus(const QString &codeId);

    //帐号服务
    /**
     * @brief sendSmsCode  发送短信验证码
     * @param phoneNumber  手机号码
     */
    QNetworkReply* sendSmsCode(const QString &phoneNumber, const QString &accessToken);

    /**
     * @brief verifySmsCode  检查短信验证码
     * @param phoneNumber    手机号
     * @param smsCode        短信验证码
     */
    QNetworkReply* verifySmsCode(const QString &phoneNumber, const QString &smsCode, const QString &accessToken);

    /**
     * @brief bindAccount            绑定微信
     * @param currentAccountType     当前账号类型     0:统一账号
                                                    1：微信
                                                    2:QQ
                                                    3:华为
                                                    4:手机号
                                                    5:本地账号（deviceId+账户名）
     * @param currentAccountId       当前账号标识类型  0:unionid（用户标识）
                                                    1:AT
                                                    2:sessionid
     * @param currentAccountIdValue  当前账号标识取值
     * @param bindAccountType        绑定账号类型     0:统一账号
                                                    1：微信
                                                    2:QQ
                                                    3:华为
                                                    4:手机号
                                                    5:本地账号（deviceId+账户名）
     * @param bindAccountId          绑定账号标识类型  0:unionid（用户标识）
                                                    1:AT
                                                    2:sessionid
     * @param bindAccountIdValue     绑定账号标识取值
     * @param attribute              昵称
     */
    QNetworkReply* bindAccount(const int &currentAccountType, const int &currentAccountId, const QString &currentAccountIdValue,
                               const int &bindAccountType, const int &bindAccountId, const QString &bindAccountIdValue,
                               const QString &attribute);
    /**
     * @brief unbindAccount           解绑微信
     * @param platformType            平台类型
     * @param currentAccountType      当前账号类型      0:统一账号
                                                      1：微信
                                                      2:QQ
                                                      3:华为
     * @param currentAccountId       当前账号标识类型   1:AT
                                                      2:sessionid
     * @param currentAccountIdValue  当前账号标识取值
     * @param bindAccountType         绑定账号类型      0:统一账号
                                                      1：微信
                                                      2:QQ
                                                      3:华为
     * @param bindAccountId          绑定账号标识类型   1:AT
                                                      2:sessionid
     * @param bindAccountIdValue     绑定账号标识取值
     */
    QNetworkReply* unbindAccount(const int &currentAccountType, const int &currentAccountId, const QString &currentAccountIdValue
                       , const int &bindAccountType, const int &bindAccountId, const QString &bindAccountIdValue);


    QNetworkReply* getBindAccountInfo(const int &accountType, const int &id, const QString &idValue);

    /**
     * @brief getAccessToken  获取凭证token
     * @param clientId        端侧ID
     * @param code            授权Code
     * @return
     */
    QNetworkReply* getAccessToken(const QString &clientId, const QString &code);

    QNetworkReply* refreshAccessToken(const QString &clientId, const QString &refreshtoken);

    //wuhan
    QNetworkReply *getUserInfo(const QString &accessToken);

    QNetworkReply *getPictureFromUrl(const QString &url);

public:
    QByteArray checkReply(QNetworkReply *pReply);
    bool solveJson(const QString &strJson);
    bool checkJson(const QString &strJson, QJsonObject &jsonObj);

    QString getClientId();
    QString getRedirecUrl();
    QString getRequestUrl();

Q_SIGNALS:
    void toTellGetAccessTokenFinished(QByteArray);
    void toTellGetPictureFromUrlFinished(QByteArray);
    void toTellBindAccountInfoFinished(QByteArray);
    void toTellRefreshAccessTokenFinished(QByteArray);

public Q_SLOTS:
    void onGetAccessToken(const QString &code);
    void onGetPictureFromUrl(const QString &avatarUrl);
    void onBindAccountInfo(const int &accountType, const int &id, const QString &idValue);
    void onRefreshAccessToken(const QString &clientId, const QString &refreshtoken);

private Q_SLOTS:
    void onGetAccessTokenFinished();
    void onGetPictureFromUrlFinished();
    void onBindAccountInfoFinished();
    void onRefreshAccessTokenFinished();

private:
    void loadDeepinDev();
    QString userAgentInfo();
    QString fileContent(const QString &fileName);
    void judgeClienid();
    QNetworkReply* httpRequset(const QString &type, const QNetworkRequest &requset, const QByteArray &body);

private:
    Q_DISABLE_COPY(HttpClient)

    QNetworkRequest setNetWorkRequest(const QString &requestApi, QNetworkRequest::KnownHeaders headerType = QNetworkRequest::ContentTypeHeader, QVariant headerValue = "application/json");

    QNetworkAccessManager *manager = nullptr;
    QString m_clientid;
    QString m_redirec_url;
    QString m_request_url;
};

#endif // REQUESTSERVICE_H
