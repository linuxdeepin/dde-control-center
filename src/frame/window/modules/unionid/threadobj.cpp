#include "threadobj.h"
#include <QDBusInterface>
#include <QDebug>

#include "modules/unionid/httpclient.h"

ThreadObj::ThreadObj(QObject *parent) : QObject(parent)
{

}

void ThreadObj::onLoginUser()
{
    qInfo() << "loginUser";

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
    qInfo() << "loginUser  Authorize";
}

void ThreadObj::onLogoutUser(QString qstrAvatar, QString qstrNickName)
{
    qInfo() << "logoutUser ConfirmLogout";
    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QList<QVariant> argumentList;
    argumentList << qstrAvatar;
    argumentList << qstrNickName;

    interface.callWithArgumentList(QDBus::NoBlock, "ConfirmLogout", argumentList);
    qInfo() << "logoutUser ConfirmLogout finished";
}
