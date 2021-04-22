#include "notificationmanager.h"
#include "modules/unionid/httpclient.h"
#include "define.h"
#include "window/modules/unionid/pages/avatarwidget.h"

#include <QVariantMap>
#include <QDBusArgument>
#include <QNetworkReply>
#include <QApplication>

Q_GLOBAL_STATIC(Notificationmanager, NotifiManager)

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

const QString EXCLAMATIONPATH = ":/themes/light/icons/exclamation_24px.svg";

Notificationmanager::Notificationmanager(QObject *parent) : QObject(parent)
{
    windowPosition = QPoint();
    m_bIsNotificationExist = false;
    m_refreshTimer = new QTimer;
    connect(m_refreshTimer, &QTimer::timeout, this, &Notificationmanager::onTokenTimeout);  
    m_model = new dcc::unionid::UnionidModel(nullptr);
    m_worker = new dcc::unionid::UnionidWorker(m_model);
    connect(this, &Notificationmanager::toTellLoginUser, m_worker, &dcc::unionid::UnionidWorker::loginUser);
}

Notificationmanager *Notificationmanager::instance()
{
    return NotifiManager;
}

void Notificationmanager::showToast(QWidget *parent, ErrorType type)
{
    if (m_bIsNotificationExist) {
        return;
    }

    m_message = new CustomFloatingMessage(CustomFloatingMessage::TransientType, parent);
    m_message->setIcon(EXCLAMATIONPATH);
    m_bIsNotificationExist = true;

    if( ErrorType::SystemError == type){
        m_message->setMessage(QObject::tr("system error"));
    }else if( ErrorType::NetworkError == type ){
        m_message->setMessage(QObject::tr("Network error"));
    }else if( ErrorType::ConnectionError == type ){
        m_message->setMessage(QObject::tr("connection error"));
    }else if( ErrorType::ConnectionTimeout == type ){
        m_message->setMessage(QObject::tr("connection timeout"));
    }else {
        m_message->setMessage(QObject::tr("Network error, connection timed out"));
    }
    m_message->setDuration(2000);
    m_message->adjustSize();
    m_message->move((parent->width() - m_message->width()) / 2, (parent->height() - m_message->height()) / 2);
    m_message->show();
}

void Notificationmanager::setWindowPosition(QPoint pos)
{
    windowPosition = pos;
}

QPoint Notificationmanager::getWindowPosition() const
{
    return windowPosition;
}

bool Notificationmanager::isOnLine()
{
    QDBusInterface interface("com.deepin.daemon.Network","/com/deepin/daemon/Network","com.deepin.daemon.Network");

    QString retVal = interface.property("ActiveConnections").toString();

    if (retVal == "{}") {
        return false;
    }

    return true;
}

void Notificationmanager::setNotificationStatus()
{
    m_bIsNotificationExist = false;
}

void Notificationmanager::setUserInfo(QString usrInfo)
{
    m_userInfo = usrInfo;
    QByteArray byteJson = usrInfo.toLocal8Bit();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
    QJsonObject jsonObj = jsonDoc.object();
    QJsonValue jsonValueResult = jsonObj.value("data");

    if (jsonValueResult.isObject()) {
        jsonObj = jsonValueResult.toObject();
        jsonValueResult = jsonObj.value("avatar");
        QString avatar = jsonValueResult.toString();
        AvatarWidget *uavatar = new AvatarWidget;
        uavatar->setAvatarPath(avatar);
        uavatar->deleteLater();
    }
}

QString Notificationmanager::getUserInfo()
{
    return m_userInfo;
}

void Notificationmanager::getAccessToken(const QString &code, const QString &state)
{
    Q_UNUSED(state)
    QNetworkReply *reply = HttpClient::instance()->getAccessToken(CLIENT_ID,code);
    connect(reply,&QNetworkReply::finished,this,&Notificationmanager::onGetAccessToken);
}

void Notificationmanager::startRefreshToken(const QString &refreshToken,int expires_in)
{
    m_refreshToken = refreshToken;

    if (m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }

    m_refreshTimer->start(expires_in);
}

QPixmap Notificationmanager::getUserAvatar()
{
    return m_avatar;
}

void Notificationmanager::onUserAvatar(QPixmap avatar)
{
    m_avatar = avatar;
}

void Notificationmanager::onGetAccessToken()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (HttpClient::instance()->solveJson(result)) {
        setUserInfo(result);
    }
}

void Notificationmanager::onTokenTimeout()
{
    QNetworkReply *reply = HttpClient::instance()->refreshAccessToken(CLIENT_ID,m_refreshToken);
    connect(reply,&QNetworkReply::finished,this,&Notificationmanager::onRefreshAccessToken);
}

void Notificationmanager::onRefreshAccessToken()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (HttpClient::instance()->solveJson(result)) {
        setUserInfo(result);
    }
}
