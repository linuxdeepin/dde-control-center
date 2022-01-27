#include "syncworker.h"
#include "widgets/utils.h"

#include <QProcess>
#include <QDBusConnection>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <DSysInfo>
#include <unistd.h>
#include <org_freedesktop_hostname1.h>
#include <ddbussender.h>

DCORE_USE_NAMESPACE

using namespace dcc;
using namespace dcc::cloudsync;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";

SyncWorker::SyncWorker(SyncModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
    , m_syncHelperInter(new QDBusInterface("com.deepin.sync.Helper", "/com/deepin/sync/Helper", "com.deepin.sync.Helper", QDBusConnection::systemBus(), this))
{
    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);

    QDBusConnection::systemBus().connect("com.deepin.license", "/com/deepin/license/Info",
                                         "com.deepin.license.Info", "LicenseStateChange",
                                         this, SLOT(licenseStateChangeSlot()));

    connect(m_syncInter, &SyncInter::StateChanged, this, &SyncWorker::onStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::LastSyncTimeChanged, this, &SyncWorker::onLastSyncTimeChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::SwitcherChange, this, &SyncWorker::onSyncModuleStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::UserInfoChanged, m_model, &SyncModel::setUserinfo, Qt::QueuedConnection);

    auto req = m_syncInter->isValid();

    m_model->setSyncIsValid(req && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));
    connect(m_syncInter, &DeepinId::serviceValidChanged, this, [=](bool valid) {
        m_model->setSyncIsValid(valid && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));
    });
    licenseStateChangeSlot();

    m_model->setUserinfo(m_syncInter->userInfo());
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
    QFutureWatcher<QJsonObject> *watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, this, [=] {
        watcher->deleteLater();
        QJsonObject obj = watcher->result();

        if (obj.isEmpty()) {
            qDebug() << "Sync Info is Wrong!";
            return;
        }

        m_model->setEnableSync(obj["enabled"].toBool());

        const std::list<std::pair<SyncType, QStringList>> moduleMap{
            m_model->moduleMap()
        };

        for (auto it = moduleMap.cbegin(); it != moduleMap.cend(); ++it) {
            m_model->setModuleSyncState(it->first, obj[it->second.first()].toBool());
        }
    });

    QFuture<QJsonObject> future = QtConcurrent::run([=]() -> QJsonObject {
        QDBusPendingReply<QString> reply = m_syncInter->SwitcherDump();
        reply.waitForFinished();
        return QJsonDocument::fromJson(reply.value().toUtf8()).object();
    });

    watcher->setFuture(future);
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

void SyncWorker::loginUser()
{
    m_deepinId_inter->Login();
}

void SyncWorker::logoutUser()
{
    m_deepinId_inter->Logout();
}

void SyncWorker::asyncLogoutUser(const QString &ubid)
{
    if (ubid.isEmpty()) {
        m_deepinId_inter->Logout();
        return;
    }
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT unBindRet(result.ret);
        else
            Q_EMIT resetPasswdError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::logout, ubid);
    watcher->setFuture(future);
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

void SyncWorker::onLastSyncTimeChanged(qlonglong lastSyncTime)
{
    qDebug() << "lastSyncTime: " << lastSyncTime;
    m_model->setLastSyncTime(lastSyncTime);

}

void SyncWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher,
            &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &SyncWorker::getLicenseState);
    watcher->setFuture(future);
}

void SyncWorker::getUOSID(QString &uosid)
{
    if (!m_syncHelperInter->isValid()) {
        qWarning() << "syncHelper interface invalid: (getUOSID)" << m_syncHelperInter->lastError().message();
        return;
    }
    QDBusReply<QString> retUOSID = m_syncHelperInter->call("UOSID");
    if (retUOSID.error().message().isEmpty()) {
        uosid = retUOSID.value();
    } else {
        qWarning() << "UOSID failed:" << retUOSID.error().message();
    }
}

void SyncWorker::getUUID(QString &uuid)
{
    QDBusInterface accountsInter("com.deepin.daemon.Accounts",
                                 QString("/com/deepin/daemon/Accounts/User%1").arg(getuid()),
                                 "com.deepin.daemon.Accounts.User",
                                 QDBusConnection::systemBus());
    if (!accountsInter.isValid()) {
        qWarning() << "accounts interface invalid: (getUUID)" << accountsInter.lastError().message();
        return;
    }
    QVariant retUUID = accountsInter.property("UUID");
    uuid = retUUID.toString();
}

void SyncWorker::asyncLocalBindCheck(const QString &uosid, const QString &uuid)
{
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT ubid(result.ubid);
        else
            Q_EMIT resetPasswdError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::checkLocalBind, uosid, uuid);
    watcher->setFuture(future);
}

void SyncWorker::getHostName(QString &hostName)
{
    org::freedesktop::hostname1 hostnameInter("org.freedesktop.hostname1",
                                              "/org/freedesktop/hostname1",
                                              QDBusConnection::systemBus());
    hostName = hostnameInter.staticHostname();
}

void SyncWorker::asyncBindAccount(const QString &uuid, const QString &hostName)
{
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT ubid(result.ubid);
        else
            Q_EMIT resetPasswdError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::bindAccount, uuid, hostName);
    watcher->setFuture(future);
}

void SyncWorker::asyncUnbindAccount(const QString &ubid)
{
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT unBindRet(result.ret);
        else
            Q_EMIT resetPasswdError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::unBindAccount, ubid);
    watcher->setFuture(future);
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
    m_model->setActivation(reply >= 1 && reply <= 3);
}

BindCheckResult SyncWorker::logout(const QString &ubid)
{
    BindCheckResult result = unBindAccount(ubid);
    m_deepinId_inter->Logout();
    return result;
}

BindCheckResult SyncWorker::checkLocalBind(const QString &uosid, const QString &uuid)
{
    BindCheckResult result;
    QDBusReply<QString> retLocalBindCheck= m_syncHelperInter->call(QDBus::BlockWithGui, "LocalBindCheck", uosid, uuid);
    if (!m_syncHelperInter->isValid()) {
        qWarning() << "syncHelper interface invalid: (localBindCheck)" << m_syncHelperInter->lastError().message();
        return result;
    }
    if (retLocalBindCheck.error().message().isEmpty()) {
        result.ubid = retLocalBindCheck.value();
    } else {
        qWarning() << "localBindCheck failed:" << retLocalBindCheck.error().message();
        result.error = retLocalBindCheck.error().message();
    }
    return result;
}

BindCheckResult SyncWorker::bindAccount(const QString &uuid, const QString &hostName)
{
    BindCheckResult result;
    QDBusPendingReply<QString> retUBID = DDBusSender()
                                         .service("com.deepin.deepinid")
                                         .interface("com.deepin.deepinid")
                                         .path("/com/deepin/deepinid")
                                         .method("BindLocalUUid").arg(uuid).arg(hostName)
                                         .call();
    retUBID.waitForFinished();
    if (retUBID.error().message().isEmpty()) {
        qDebug() << "Bind success!";
        result.ubid = retUBID.value();
    } else {
        qWarning() << "Bind failed:" << retUBID.error().message();
        result.error = retUBID.error().message();
    }
    return result;
}

BindCheckResult SyncWorker::unBindAccount(const QString &ubid)
{
    BindCheckResult result;
    QDBusPendingReply<QString> retUnBoundle = DDBusSender()
                                              .service("com.deepin.deepinid")
                                              .interface("com.deepin.deepinid")
                                              .path("/com/deepin/deepinid")
                                              .method("UnBindLocalUUid").arg(ubid)
                                              .call();
    retUnBoundle.waitForFinished();
    if (retUnBoundle.error().message().isEmpty()) {
        qDebug() << "unBind success!";
        result.ret = true;
    } else {
        qWarning() << "unBind failed:" << retUnBoundle.error().message();
        result.error = retUnBoundle.error().message();
        result.ret = false;
    }
    return result;
}
