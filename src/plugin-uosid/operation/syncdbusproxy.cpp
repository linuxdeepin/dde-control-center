#include "syncdbusproxy.h"
#include "widgets/dccdbusinterface.h"

const static QString SyncService = QStringLiteral("com.deepin.sync.Daemon");
const static QString SyncPath = QStringLiteral("/com/deepin/sync/Daemon");
const static QString SyncInterface = QStringLiteral("com.deepin.sync.Daemon");

const static QString DeepinIDService = QStringLiteral("com.deepin.deepinid");
const static QString DeepinIDPath = QStringLiteral("/com/deepin/deepinid");
const static QString DeepinIDInterface = QStringLiteral("com.deepin.deepinid");

const static QString SyncHelperService = QStringLiteral("com.deepin.sync.Helper");
const static QString SyncHelperPath = QStringLiteral("/com/deepin/sync/Helper");
const static QString SyncHelperInterface = QStringLiteral("com.deepin.sync.Helper");

const static QString Hostname1Service = QStringLiteral("org.freedesktop.hostname1");
const static QString Hostname1Path = QStringLiteral("/org/freedesktop/hostname1");
const static QString Hostname1Interface = QStringLiteral("org.freedesktop.hostname1");

DCC_USE_NAMESPACE
SyncDBusProxy::SyncDBusProxy(QObject *parent)
    : QObject(parent)
    , m_syncInter(new DCCDBusInterface(SyncService, SyncPath, SyncInterface, QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DCCDBusInterface(DeepinIDService, DeepinIDPath, DeepinIDInterface, QDBusConnection::sessionBus(), this))
    , m_syncHelperInter(new DCCDBusInterface(SyncHelperService, SyncHelperPath, SyncHelperInterface, QDBusConnection::systemBus(), this))
    , m_hostnameInter(new DCCDBusInterface(Hostname1Service, Hostname1Path, Hostname1Interface, QDBusConnection::systemBus(), this))
{
    qRegisterMetaType<IntString>("IntString");
    qDBusRegisterMetaType<IntString>();
    m_deepinId_inter->setSuffix("Deepinid");
    initConnent();
}

void SyncDBusProxy::initConnent()
{
    connect(m_syncInter, SIGNAL(SwitcherChange(QString,bool)), this, SIGNAL(SwitcherChange(QString,bool)));
}

qlonglong SyncDBusProxy::lastSyncTime()
{
    return qvariant_cast<qlonglong>(m_syncInter->internalPropGet("LastSyncTime"));
}

IntString SyncDBusProxy::state()
{
    return qvariant_cast<IntString>(m_syncInter->internalPropGet("State"));
}

QString SyncDBusProxy::SwitcherDump()
{
    QDBusPendingReply<QString> reply = m_syncInter->asyncCall(QStringLiteral("SwitcherDump"));
    reply.waitForFinished();
    if (reply.isError()) {
        qInfo() << " GET syncdeamon SwitcherDump Error!";
        return QString();
    } else {
        return reply.value();
    }
}

void SyncDBusProxy::SwitcherSet(const QString &in0, bool in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    m_syncInter->asyncCallWithArgumentList(QStringLiteral("SwitcherSet"), argumentList);
}

QVariantMap SyncDBusProxy::userInfo()
{
    return qvariant_cast<QVariantMap>(m_deepinId_inter->internalPropGet("UserInfoDeepinid"));
}

QDBusPendingReply<QString> SyncDBusProxy::LocalBindCheck(const QString &uuid)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(uuid);
    return m_deepinId_inter->asyncCallWithArgumentList(QStringLiteral("LocalBindCheck"), argumentList);
}

void SyncDBusProxy::Login()
{
    QList<QVariant> argumentList;
    m_deepinId_inter->asyncCallWithArgumentList(QStringLiteral("Login"), argumentList);
}

void SyncDBusProxy::Logout()
{
    QList<QVariant> argumentList;
    m_deepinId_inter->asyncCallWithArgumentList(QStringLiteral("Logout"), argumentList);
}

QString SyncDBusProxy::UOSID()
{
    QDBusPendingReply<QString> reply = m_syncHelperInter->asyncCall(QStringLiteral("UOSID"));
    reply.waitForFinished();
    if (reply.isError()) {
        qInfo() << " GET syncdeamon SwitcherDump Error!";
        return QString();
    } else {
        return reply.value();
    }
}

QString SyncDBusProxy::StaticHostname()
{
    return qvariant_cast<QString>(m_hostnameInter->internalPropGet("StaticHostname"));
}
