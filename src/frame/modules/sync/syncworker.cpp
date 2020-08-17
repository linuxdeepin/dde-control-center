#include "syncworker.h"
#include "widgets/utils.h"

#include <QProcess>
#include <QDBusConnection>
#include <DSysInfo>

DCORE_USE_NAMESPACE

using namespace dcc;
using namespace dcc::cloudsync;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";

SyncWorker::SyncWorker(SyncModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
{
    //采用的是DBus直接获取属性值的方式
    QDBusInterface Interface("com.deepin.sync.Daemon",
                        "/com/deepin/sync/Daemon",
                        "org.freedesktop.DBus.Properties",
                        QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("Get", "com.deepin.sync.Daemon", "UserInfo");
    QVariant variant = reply.arguments().first();
    QDBusArgument argument = variant.value<QDBusVariant>().variant().value<QDBusArgument>();
    qDebug() << argument.currentSignature() << argument.currentType();
    QVariantMap userInfo;
    argument >> userInfo;
    m_model->setUserinfo(userInfo);

    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);

    QDBusConnection::sessionBus().connect("com.deepin.sync.Daemon",
                                          "/com/deepin/sync/Daemon",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(userInfoChanged(QDBusMessage)));

    m_activeInfo = new QDBusInterface("com.deepin.license",
                                      "/com/deepin/license/Info",
                                      "com.deepin.license.Info",
                                      QDBusConnection::systemBus(),this);

    connect(m_activeInfo, SIGNAL(LicenseStateChange()),this, SLOT(licenseStateChangeSlot()));
    connect(m_syncInter, &SyncInter::StateChanged, this, &SyncWorker::onStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::LastSyncTimeChanged, this, &SyncWorker::onLastSyncTimeChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::SwitcherChange, this, &SyncWorker::onSyncModuleStateChanged, Qt::QueuedConnection);

    auto req = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.deepinid");

    m_model->setSyncIsValid(req.value() && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));
    getLicenseState();
}

void SyncWorker::activate()
{
    m_syncInter->blockSignals(false);
    m_deepinId_inter->blockSignals(false);

    onStateChanged(m_syncInter->state());
    onLastSyncTimeChanged(m_syncInter->lastSyncTime());

    refreshSyncState();
}

void SyncWorker::deactivate()
{
    m_syncInter->blockSignals(true);
    m_deepinId_inter->blockSignals(true);
}

void SyncWorker::refreshSyncState()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_syncInter->SwitcherDump(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &SyncWorker::onGetModuleSyncStateFinished);
    watcher->waitForFinished();
}

void SyncWorker::setSync(std::pair<SyncType, bool> state)
{
    // TODO(justforlxz): Maybe will add screensaver in the future
    // you don't need a multimap.
    const std::list<std::pair<SyncType, QStringList>> map { m_model->moduleMap() };
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        if (it->first == state.first) {
            for (const QString &value : it->second) {
                m_syncInter->SwitcherSet(value, state.second);
            }
        }
    }
}

void SyncWorker::userInfoChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    if (changedProps.empty() || changedProps.keys().first().compare("UserInfo")) {
        qDebug() << "userInfoChanged changedProps=" << changedProps;
        return;
    }
    QVariantMap userInfo;
    QDBusArgument arg = changedProps.value("UserInfo").value<QDBusArgument>();
    arg >> userInfo;
    qDebug() << userInfo;
    m_model->setUserinfo(userInfo);
}

void SyncWorker::loginUser()
{
    m_deepinId_inter->Login();
}

void SyncWorker::logoutUser()
{
    m_deepinId_inter->Logout();
}

void SyncWorker::setAutoSync(bool autoSync)
{
    m_syncInter->SwitcherSet("enabled", autoSync);
}

void SyncWorker::onSyncModuleStateChanged(const QString &module, bool enable)
{
    if (module == "enabled") {
        return m_model->setEnableSync(enable);
    }

    const std::list<std::pair<SyncType, QStringList>> list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        if (it->second.contains(module)) {
            m_model->setModuleSyncState(it->first, enable);
            break;
        }
    }
}

void SyncWorker::onStateChanged(const IntString &state)
{
    std::pair<qint32, QString> value(state.state, state.description);
    if (SyncModel::isSyncStateValid(value)) {
        qDebug() << "activate: " << state.description;
        m_model->setSyncState(value);
    }
}

void SyncWorker::onGetModuleSyncStateFinished(QDBusPendingCallWatcher *watcher)
{
    watcher->deleteLater();

    if (watcher->isError()) {
        qWarning() << watcher->error();
        return;
    }

    QDBusReply<QString> reply = watcher->reply();
    QJsonObject obj = QJsonDocument::fromJson(reply.value().toUtf8()).object();

    if (obj.isEmpty()) {
        qWarning() << "Sync Info is Wrong!";
        return;
    }

    m_model->setEnableSync(obj["enabled"].toBool());

    const std::list<std::pair<SyncType, QStringList>> moduleMap = m_model->moduleMap();
    for (auto it = moduleMap.cbegin(); it != moduleMap.cend(); ++it) {
        m_model->setModuleSyncState(it->first, obj[it->second.first()].toBool());
    }
}

void SyncWorker::onLastSyncTimeChanged(qlonglong lastSyncTime)
{
    qDebug() << "lastSyncTime: " << lastSyncTime;
    m_model->setLastSyncTime(lastSyncTime);

}

void SyncWorker::licenseStateChangeSlot()
{
    getLicenseState();
}

void SyncWorker::getLicenseState()
{
    if (DSysInfo::DeepinDesktop == DSysInfo::deepinType()) {
        m_model->setActivation(true);
        return;
    }
    
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }

    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "authorize result:" << reply;
    m_model->setActivation(reply == 1 || reply == 3);
}
