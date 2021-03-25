#ifndef REQUESTSERVICE_H
#define REQUESTSERVICE_H

#include <QObject>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include "shttpsocket.h"

enum REQURST_RESULT
{
    REQURST_SUCCESS,
    REQURST_FAIL
};

class RequestService : public QObject
{
    Q_OBJECT
public:
    explicit RequestService(QObject *parent = nullptr);

    static RequestService* instance();
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
     */
    QNetworkReply* requestQrCode(const QString clientId, const QString deviceId, const int scene, const QString redirectUrl);

    /**
     * @brief reportQrCodeStatus  上报二维码状态
     * @param codeId              UUID生成唯一序列
     * @param qrCodeStatus           扫码状态         1.已扫码
                                                    2.扫码成功-已绑定
                                                    3.扫码成功-未绑定
                                                    4.已完成
     * @param role                扫码标志           1：消费端（手机扫码）
                                                    2：生产端（PC）
     * @param entityId            实体对象值         具体扫码设备ID
     */
    QNetworkReply* reportQrCodeStatus(const QString codeId, const int qrCodeStatus);

    /**
     * @brief getQrCode  获取二维码属性
     * @param codeId     UUID生成唯一序列
     */
    QNetworkReply* getQrCode(const QString codeId);

    /**
     * @brief getQrCodeStatus  查询扫码状态
     * @param codeId           UUID生成唯一序列
     * @param role             扫码标志           1：消费端（手机扫码）
                                                 2：生产端（PC）
     * @param entityId         实体对象值         具体扫码设备ID
     */
    QNetworkReply* getQrCodeStatus(const QString codeId);

    //帐号服务
    /**
     * @brief login        登录统信账号
     * @param type         登入类型         (web/uos/mobile)
     * @param redirectUrl  注册应用回调地址   公司开放平台填写的回调地址
     * @param loginType    认证方式          0:微信扫码  //mobile
                                            1：账号密码 //PC
                                            2：seession认证
     * @param account      账号
     * @param password     密码
     */
    QNetworkReply* login(const QString type, const QString redirectUrl, const QString loginType
               , const QString account, const QString password);

    /**
     * @brief createAccount  注册统信账号
     * @param nickname       昵称
     * @param password       密码
     * @param phone          手机号
     * @return
     */
    QNetworkReply* createAccount(const QString nickname, const QString password, const QString phone);

    /**
     * @brief coed2Session  授权码登录
     * @param accountType   账号类型   0:统信账号
                                      1：微信
                                      2:QQ
                                      3:华为
                                      4:手机号
                                      5:本地账号
     * @param code          授权码code
     */
    QNetworkReply* coed2Session(const QString accountType, const QString code);

    /**
     * @brief getBoundAccount  获取用户绑定的账号
     * @param accountType      账号类型          0:统一账号
                                                1：微信
                                                2:QQ
                                                3:华为
                                                4:手机号
                                                5:本地账号
     * @param id               标识类型          0:unionid（用户标识）
                                                1:AT
                                                2:sessionid
     * @param idValue          标识取值
     */
    QNetworkReply* getBoundAccount(const QString accountType, const QString id, const QString idValue);

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
     */
    QNetworkReply* bindAccount(const int currentAccountType, const int currentAccountId, const QString currentAccountIdValue
                     , const int bindAccountType, const int bindAccountId, const QString bindAccountIdValue);
    /**
     * @brief unbindAccount           解绑微信
     * @param platformType            平台类型
     * @param currentAccountType      当前账号类型      0:统一账号
                                                      1：微信
                                                      2:QQ
                                                      3:华为
     * @param currentAccountTag       当前账号标识类型   1:AT
                                                      2:sessionid
     * @param currentAccountTagValue  当前账号标识取值
     * @param bindAccountType         绑定账号类型      0:统一账号
                                                      1：微信
                                                      2:QQ
                                                      3:华为
     * @param bindAccountTag          绑定账号标识类型   1:AT
                                                      2:sessionid
     * @param bindAccountTagValue     绑定账号标识取值
     */
    QNetworkReply* unbindAccount(const int currentAccountType, const int currentAccountTag, const QString currentAccountTagValue
                       , const int bindAccountType, const int bindAccountTag, const QString bindAccountTagValue);

    /**
     * @brief redirectAuthInfo  获取微信认证状态
     * @param code              授权Code
     * @param state             状态
     * @param app               Path参数         app-store
     *                                          uid-managment
     *                                          sys-active-promotion
     */
    QNetworkReply* redirectAuthInfo(const QString code, const QString state, const QString app);

    /**
     * @brief getAccessToken  获取凭证token
     * @param clientId        端侧ID
     * @param code            授权Code
     * @return
     */
    QNetworkReply* getAccessToken(const QString clientId, const QString code);

    QNetworkReply* refreshAccessToken(const QString clientId, const QString refreshtoken);

    /**
     * @brief decrptData    微信数据解密
     * @param encrytedData  加密串
     * @param iv            初始化向量
     * @param sessionId     会话标志
     */
    QNetworkReply* decrptData(const QString encrytedData, const QString iv, const QString sessionId);

    //激活服务
    /**
     * @brief queryUserPromotionResult  获取用户激活状态
     * @param unionId                   统一账号标识
     */   
    QNetworkReply* queryUserPromotionResult(const QString unionId);

    /**
     * @brief reportUserPromotionResult
     * @param unionId                    统一账号标识
     * @param sessionId                  会话Id(UUID)
     */
    QNetworkReply* reportUserPromotionResult(const QString unionId, const QString sessionId);

    //wuhan
    QNetworkReply *getUserInfo(const QString &accessToken);

public:
    QString checkReply(QNetworkReply *pReply);

    bool solveJson(QString strJson);

    bool checkJson(QString strJson, QJsonObject &jsonObj);

private:
    Q_DISABLE_COPY(RequestService)

};

#endif // REQUESTSERVICE_H
