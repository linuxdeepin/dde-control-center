#include "threadobj.h"
#include <QDBusInterface>
#include <QDebug>

#include "httpclient.h"

ThreadObj::ThreadObj(QObject *parent) : QObject(parent)
{

}

void ThreadObj::onLoginUser()
{
    qInfo() << "loginUser begin";

    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QVariant clientId = HttpClient::instance()->getClientId();
    QVariant redirecUrl = HttpClient::instance()->getRedirecUrl();
    QList<QVariant> argumentList;
    argumentList << clientId;
    argumentList << "com.deepin.dde.ControlCenter";
    argumentList << "/com/deepin/dde/ControlCenter";
    argumentList << "com.deepin.dde.ControlCenter";
    interface.callWithArgumentList(QDBus::NoBlock, "Register", argumentList);
    qInfo() << "loginUser  Register";

    argumentList = {};
    argumentList << clientId;
    argumentList << QStringList{"base","user.api:contact","user:contact:read"};
    argumentList << redirecUrl;
    argumentList << "state";
    interface.callWithArgumentList(QDBus::NoBlock, "Authorize", argumentList);

    Q_EMIT toTellLoginUserFinished();
    qInfo() << "loginUser end";
}

void ThreadObj::onLogoutUser(QString qstrAvatar, QString qstrNickName)
{
    qInfo() << "logoutUser begin";
    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QList<QVariant> argumentList;
    argumentList << qstrAvatar;
    argumentList << qstrNickName;

    QDBusMessage msg = interface.callWithArgumentList(QDBus::NoBlock, "ConfirmLogout", argumentList);

    if (msg.type() == QDBusMessage::ReplyMessage) {
        bool bIsSuccess = msg.arguments().takeFirst().toBool();

        if (bIsSuccess) {
            qInfo() << "退出登录成功";
        } else {
            qInfo() << "退出登录失败";
        }
    }
    qInfo() << "logoutUser end";
}
