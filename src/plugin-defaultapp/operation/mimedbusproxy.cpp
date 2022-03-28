#include "mimedbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const static QString MimeService = "com.deepin.daemon.Mime";
const static QString MimePath = "/com/deepin/daemon/Mime";
const static QString MimeInterface = "com.deepin.daemon.Mime";

MimeDBusProxy::MimeDBusProxy(QObject *parent)
    : QObject(parent)
{
    m_mimeInter = new QDBusInterface(MimeService, MimePath, MimeInterface, QDBusConnection::sessionBus(), this);
    connect(m_mimeInter,SIGNAL(Change()),this,SIGNAL(Change()),Qt::QueuedConnection);
}

void MimeDBusProxy::SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(mimeTypes) << QVariant::fromValue(desktopId);
    m_mimeInter->asyncCallWithArgumentList("SetDefaultApp", argumentList);
}

void MimeDBusProxy::DeleteApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteApp", mimeTypes, desktopId));
}

void MimeDBusProxy::DeleteUserApp(const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteUserApp", desktopId));
}

void MimeDBusProxy::AddUserApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("AddUserApp", mimeTypes, desktopId));
}

QString MimeDBusProxy::GetDefaultApp(const QString &mimeType)
{
    return QDBusPendingReply<QString>(m_mimeInter->asyncCall("GetDefaultApp",mimeType));
}

QString MimeDBusProxy::ListApps(const QString &mimeType)
{
    return QDBusPendingReply<QString>(m_mimeInter->asyncCall("ListApps",mimeType));
}

QString MimeDBusProxy::ListUserApps(const QString &mimeType)
{
    return QDBusPendingReply<QString>(m_mimeInter->asyncCall("ListUserApps",mimeType));
}


