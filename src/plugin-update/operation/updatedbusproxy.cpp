#include "updatedbusproxy.h"

#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDBusReply>

// Updater
const static QString UpdaterService = QStringLiteral("com.deepin.lastore");
const static QString UpdaterPath = QStringLiteral("/com/deepin/lastore");
const static QString UpdaterInterface = QStringLiteral("com.deepin.lastore.Updater");

// Manager
const static QString ManagerService = QStringLiteral("com.deepin.lastore");
const static QString ManagerPath = QStringLiteral("/com/deepin/lastore");
const static QString ManagerInterface = QStringLiteral("com.deepin.lastore.Manager");

// PowerInter
const static QString PowerService = QStringLiteral("org.deepin.daemon.Power1");
const static QString PowerPath = QStringLiteral("/org/deepin/daemon/Power1");
const static QString PowerInterface = QStringLiteral("org.deepin.daemon.Power1");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

UpdateDBusProxy::UpdateDBusProxy(QObject *parent)
    : QObject(parent)
    , m_updateInter(new QDBusInterface(UpdaterService, UpdaterPath, UpdaterInterface, QDBusConnection::systemBus(), this))
    , m_managerInter(new QDBusInterface(ManagerService, ManagerPath, ManagerInterface, QDBusConnection::systemBus(), this))
    , m_powerInter(new QDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this))
{
    qRegisterMetaType<LastoreUpdatePackagesInfo>("LastoreUpdatePackagesInfo");
    qDBusRegisterMetaType<LastoreUpdatePackagesInfo>();

    qRegisterMetaType<BatteryPercentageInfo>("BatteryPercentageInfo");
    qDBusRegisterMetaType<BatteryPercentageInfo>();

    QDBusConnection::systemBus().connect(UpdaterService, UpdaterPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect(ManagerService, ManagerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(PowerService, PowerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

UpdateDBusProxy::~UpdateDBusProxy()
{
    m_updateInter->deleteLater();
    m_managerInter->deleteLater();
    m_powerInter->deleteLater();
}

bool UpdateDBusProxy::updateNotify()
{
    return qvariant_cast<bool>(m_updateInter->property("UpdateNotify"));
}

void UpdateDBusProxy::SetUpdateNotify(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetUpdateNotify"), argumentList);
}

LastoreUpdatePackagesInfo UpdateDBusProxy::classifiedUpdatablePackages()
{
    QDBusInterface updateInter(m_updateInter->service(), m_updateInter->path(), PropertiesInterface, m_updateInter->connection());
    QDBusMessage mess = updateInter.call(QStringLiteral("Get"),m_updateInter->interface(),QStringLiteral("ClassifiedUpdatablePackages"));
    QVariant v  = mess.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    LastoreUpdatePackagesInfo packagesInfo;
    arg>>packagesInfo;
    return packagesInfo;
}

double UpdateDBusProxy::GetCheckIntervalAndTime(QString &out1)
{
    QList<QVariant> argumentList;
    QDBusMessage reply = m_updateInter->callWithArgumentList(QDBus::Block, QStringLiteral("GetCheckIntervalAndTime"), argumentList);
    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
        out1 = qdbus_cast<QString>(reply.arguments().at(1));
    }
    return qdbus_cast<double>(reply.arguments().at(0));
}

bool UpdateDBusProxy::autoDownloadUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoDownloadUpdates"));
}

bool UpdateDBusProxy::autoInstallUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoInstallUpdates"));
}

qulonglong UpdateDBusProxy::autoInstallUpdateType()
{
    return qvariant_cast<qulonglong>(m_updateInter->property("AutoInstallUpdateType"));
}

bool UpdateDBusProxy::autoCheckUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoCheckUpdates"));
}

void UpdateDBusProxy::SetAutoCheckUpdates(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetAutoCheckUpdates"), argumentList);
}

void UpdateDBusProxy::SetAutoDownloadUpdates(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetAutoDownloadUpdates"), argumentList);
}

void UpdateDBusProxy::setAutoInstallUpdates(bool value)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(value);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("setAutoInstallUpdates"), argumentList);
}

void UpdateDBusProxy::SetMirrorSource(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetMirrorSource"), argumentList);
}

bool UpdateDBusProxy::autoClean()
{
    return qvariant_cast<bool>(m_managerInter->property("AutoCheckUpdates"));
}

qulonglong UpdateDBusProxy::updateMode()
{
    return qvariant_cast<qulonglong>(m_managerInter->property("UpdateMode"));
}

void UpdateDBusProxy::setUpdateMode(qulonglong value)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(value);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("SetUpdateMode"), argumentList);
}

QList<QDBusObjectPath> UpdateDBusProxy::jobList()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_managerInter->property("JobList"));
}

QDBusPendingReply<QDBusObjectPath> UpdateDBusProxy::UpdateSource()
{
    QList<QVariant> argumentList;
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("UpdateSource"), argumentList);
}

void UpdateDBusProxy::CleanJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("CleanJob"), argumentList);
}

void UpdateDBusProxy::SetAutoClean(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("SetAutoClean"), argumentList);
}

void UpdateDBusProxy::StartJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("StartJob"), argumentList);
}

void UpdateDBusProxy::PauseJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("PauseJob"), argumentList);
}

QDBusPendingReply<QList<QDBusObjectPath> > UpdateDBusProxy::ClassifiedUpgrade(qulonglong in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("ClassifiedUpgrade"), argumentList);
}

QDBusPendingReply<qlonglong> UpdateDBusProxy::PackagesDownloadSize(const QStringList &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("PackagesDownloadSize"), argumentList);
}

bool UpdateDBusProxy::onBattery()
{
    return qvariant_cast<bool>(m_powerInter->property("OnBattery"));
}

BatteryPercentageInfo UpdateDBusProxy::batteryPercentage()
{
    QDBusInterface powerInter(m_powerInter->service(), m_powerInter->path(), PropertiesInterface, m_powerInter->connection());
    QDBusMessage mess = powerInter.call(QStringLiteral("Get"),m_updateInter->interface(),QStringLiteral("BatteryPercentage"));
    QVariant v  = mess.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    BatteryPercentageInfo packagesInfo;
    arg>>packagesInfo;
    return packagesInfo;
}

void UpdateDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
