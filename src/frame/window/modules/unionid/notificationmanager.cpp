#include "notificationmanager.h"
#include "modules/unionid/httpclient.h"
#include "define.h"
#include "window/modules/unionid/pages/avatarwidget.h"

#include <QVariantMap>
#include <QDBusArgument>
#include <QNetworkReply>
#include <QApplication>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

const QString EXCLAMATIONPATH = ":/themes/light/icons/exclamation_24px.svg";

static Notificationmanager *NotifiManager = nullptr;

Notificationmanager::Notificationmanager(QObject *parent) : QObject(parent)
{
    m_bIsLogin = false;
    windowPosition = QPoint();
    m_bIsNotificationExist = false;
    m_refreshTimer = new QTimer;
    connect(m_refreshTimer, &QTimer::timeout, this, &Notificationmanager::onTokenTimeout);  

    m_timer_isconnect = new QTimer;
    m_timer_isconnect->setSingleShot(true);
    connect(m_timer_isconnect, &QTimer::timeout, this, &Notificationmanager::timeout);

    m_myping = new CustomPing;
    connect(m_myping->getProcess(), &QProcess::readyRead, this, &Notificationmanager::showResult);
    connect(m_myping->getProcess(), &QProcess::stateChanged, this, &Notificationmanager::showState);
    connect(m_myping->getProcess(), &QProcess::errorOccurred, this, &Notificationmanager::showError);
    connect(m_myping->getProcess(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slots_restartProcess(int, QProcess::ExitStatus)));
    connect(this, &Notificationmanager::ProcessFinished, m_myping, &CustomPing::slot_resetProcess);
    m_myping->start();
}

Notificationmanager *Notificationmanager::instance()
{
    if (NotifiManager == nullptr) {
        NotifiManager = new Notificationmanager;
    }

    return NotifiManager;
}

void Notificationmanager::showToast(QWidget *parent, ErrorType type, const QString &msg)
{
    if (!m_bIsNotificationExist) {
        m_message = new CustomFloatingMessage(CustomFloatingMessage::TransientType, parent);
        m_message->setIcon(EXCLAMATIONPATH);

        if (ErrorType::SystemError == type) {
            m_message->setMessage(tr("system error"));
        } else if (ErrorType::NetworkError == type) {
            m_message->setMessage(tr("Network error"));
        } else if (ErrorType::ConnectionError == type) {
            m_message->setMessage(tr("connection error"));
        } else if (ErrorType::ConnectionTimeout == type) {
            m_message->setMessage(tr("connection timeout"));
        } else {
            m_message->setMessage(msg);
        }

        m_bIsNotificationExist = true;
        m_message->setDuration(2000);
        m_message->adjustSize();
        m_message->move((parent->width() - m_message->width()) / 2, (parent->height() - m_message->height()) / 2);
        connect(m_message, &CustomFloatingMessage::signal_setFlag, this, &Notificationmanager::onSetNotificationStatus);
    }
}

void Notificationmanager::setWindowPosition(QPoint pos)
{
    windowPosition = pos;
}

QPoint Notificationmanager::getWindowPosition() const
{
    return windowPosition;
}

//bool Notificationmanager::isOnLine()
//{
//    QDBusInterface interface("com.deepin.daemon.Network","/com/deepin/daemon/Network","com.deepin.daemon.Network");

//    QString retVal = interface.property("ActiveConnections").toString();

//    if (retVal == "{}") {
//        return false;
//    }

//    return true;
//}

void Notificationmanager::onSetNotificationStatus()
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
        QNetworkReply *avatarReply = HttpClient::instance()->getPictureFromUrl(avatar);
        connect(avatarReply, &QNetworkReply::finished, this, &Notificationmanager::readAvatarFromUrl);

        jsonValueResult = jsonObj.value("wechatunionid");
        QString weChatUnionId = jsonValueResult.toString();

        QNetworkReply *reply =  HttpClient::instance()->getBindAccountInfo(1, 0, weChatUnionId);
        connect(reply,&QNetworkReply::finished,this,&Notificationmanager::onGetBindAccountInfo);
    }
}

QString Notificationmanager::getUserInfo()
{
    return m_userInfo;
}

void Notificationmanager::setWeChatName(QString weChatName)
{
    m_weChatName = weChatName;
}

QString Notificationmanager::getWeChatName()
{
    return m_weChatName;
}

void Notificationmanager::getAccessToken(const QString &code, const QString &state)
{
    Q_UNUSED(state)
    QNetworkReply *reply = HttpClient::instance()->getAccessToken(HttpClient::instance()->getClientId(),code);
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

void Notificationmanager::setFirstLogin()
{
    m_bIsLogin = true;
}

bool Notificationmanager::firstIsLogin()
{
    if (m_bIsLogin) {
        m_bIsLogin = false;
        return true;
    } else {
        return m_bIsLogin;
    }
}

bool Notificationmanager::isLogin()
{
    QDBusInterface interface("com.deepin.sync.Daemon","/com/deepin/deepinid","com.deepin.deepinid");

    bool isLogin = interface.property("IsLogin").toBool();

    return isLogin;
}

void Notificationmanager::showResult()
{
    m_isConnect = true;
    m_timer_isconnect->stop();
    m_timer_isconnect->start(m_timeouttime);
}

void Notificationmanager::showState(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
    } else if (state == QProcess::Starting) {
    }  else {
        m_timer_isconnect->start(m_timeouttime);
    }
}

void Notificationmanager::showError()
{
    m_isConnect = false;
}

void Notificationmanager::timeout()
{
    m_isConnect = false;
}

void Notificationmanager::slots_restartProcess(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(status)
    m_isConnect = false;
    Q_EMIT ProcessFinished();
}

bool Notificationmanager::isOnLine()
{
    return m_isConnect;
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
    QNetworkReply *reply = HttpClient::instance()->refreshAccessToken(HttpClient::instance()->getClientId(),m_refreshToken);
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

void Notificationmanager::onGetBindAccountInfo()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {

        if (HttpClient::instance()->solveJson(result)) {
            QByteArray byteJson = result.toLocal8Bit();
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
            QJsonObject jsonObj = jsonDoc.object();
            QJsonValue jsonValueResult = jsonObj.value("data");

            if (jsonValueResult.isObject()) {
                jsonObj = jsonValueResult.toObject();
                jsonValueResult = jsonObj.value("wechatNickName");
                m_weChatName = jsonValueResult.toString();
                Q_EMIT toTellgetATFinished();
            }
        }
    }
}

void Notificationmanager::readAvatarFromUrl()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());

    QByteArray result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (!result.isEmpty()) {
        m_avatar.loadFromData(result);
    }
}
