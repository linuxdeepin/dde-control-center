#include "syncworker.h"
#include "widgets/utils.h"

#include <QProcess>
#include <QDBusConnection>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <DSysInfo>
#include <unistd.h>
#include <ddbussender.h>

DCORE_USE_NAMESPACE

SyncWorker::SyncWorker(SyncModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncDBusProxy(this))
 {
    QDBusConnection::systemBus().connect("com.deepin.license", "/com/deepin/license/Info",
                                         "com.deepin.license.Info", "LicenseStateChange",
                                         this, SLOT(licenseStateChangeSlot()));

    connect(m_syncInter, &SyncDBusProxy::StateChanged, this, &SyncWorker::onStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncDBusProxy::LastSyncTimeChanged, this, &SyncWorker::onLastSyncTimeChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncDBusProxy::SwitcherChange, this, &SyncWorker::onSyncModuleStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncDBusProxy::UserInfoDeepinidChanged, m_model, &SyncModel::setUserinfo, Qt::QueuedConnection);

    getUserDeepinidInfo();
    licenseStateChangeSlot();
    m_model->setUserinfo(m_syncInter->userInfo());
}

void SyncWorker::activate()
{
    onStateChanged(m_syncInter->state());
    onLastSyncTimeChanged(m_syncInter->lastSyncTime());

    refreshSyncState();
}

void SyncWorker::refreshSyncState()
{
    QFutureWatcher<QJsonObject> *watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, this, [=] {
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
        watcher->deleteLater();
    });

    QFuture<QJsonObject> future = QtConcurrent::run([=]() -> QJsonObject {
        QString switcherDump = m_syncInter->SwitcherDump();
        return QJsonDocument::fromJson(switcherDump.toUtf8()).object();
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

void SyncWorker::setSyncState(const QString &syncType, bool state)
{
    qDebug() << " worker_sync Info : " << syncType << state;
    m_syncInter->SwitcherSet(syncType, state);
}

void SyncWorker::loginUser()
{
    m_syncInter->Login();
}

void SyncWorker::logoutUser()
{
    m_syncInter->Logout();
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

void SyncWorker::getUOSID()
{
    QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [=] {
        qDebug() << " getUOSID: " << watcher->result();
        m_model->setUOSID(watcher->result());
        watcher->deleteLater();
    });
    QFuture<QString> future = QtConcurrent::run( [=]() -> QString{
        QString retUOSID = m_syncInter->UOSID();
        if (retUOSID.isEmpty()) {
            qWarning() << "UOSID failed:";
        }
        return retUOSID;
    });
    watcher->setFuture(future);
}

void SyncWorker::getUUID()
{
    QDBusInterface accountsInter("com.deepin.daemon.Accounts",
                                 QString("/com/deepin/daemon/Accounts/User%1").arg(getuid()),
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::systemBus());
    if (!accountsInter.isValid()) {
        qWarning() << "accounts interface invalid: (getUUID)" << accountsInter.lastError().message();
        return;
    }

    QDBusPendingCall call = accountsInter.asyncCall("Get", "com.deepin.daemon.Accounts.User" ,"UUID");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call, watcher] {
        if (!call.isError()) {
            QDBusReply<QDBusVariant> reply = call.reply();
            qDebug() << " getUUID: " << reply.value().variant().toString();
            m_model->setUUID(reply.value().variant().toString());

        } else {
            qWarning() << "Failed to get driver info: " << call.error().message();
        }
        watcher->deleteLater();
    });
}

void SyncWorker::getHostName()
{
    m_model->setHostName(m_syncInter->StaticHostname());
}

void SyncWorker::asyncLocalBindCheck(const QString &uuid)
{
    qDebug() << "user Bind uuid" << uuid;
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty()) {
            qDebug() << "user Bind " << result.ubid;
            m_model->setBindLocalUBid(result.ubid);
        } else {
            m_model->setResetPasswdError(result.error);
        }
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::checkLocalBind, uuid);
    watcher->setFuture(future);
}


void SyncWorker::asyncBindAccount(const QString &uuid, const QString &hostName)
{
    qDebug() << "Start Bind! uuid: " << uuid << " host name: " << hostName;
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty()) {
            m_model->setBindLocalUBid(result.ubid);
        } else {
            m_model->setResetPasswdError(result.error);
        }
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::bindAccount, uuid, hostName);
    watcher->setFuture(future);
}

void SyncWorker::asyncUnbindAccount(const QString &ubid)
{
    qDebug() << "Start UnBind!";
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty()) {
            m_model->setBindLocalUBid(QString());
        }
        else {
            m_model->setResetPasswdError(result.error);
        }
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &SyncWorker::unBindAccount, ubid);
    watcher->setFuture(future);
}

void SyncWorker::asyncSetFullname(const QString &fullname)
{
    QDBusInterface utInterface("com.deepin.sync.Daemon",
                               "/com/deepin/utcloud/Daemon",
                               "com.deepin.utcloud.Daemon"
                               ,QDBusConnection::sessionBus());
    QDBusReply<QString> retFullName = utInterface.call("SetNickname", fullname);
    if (retFullName.error().message().isEmpty()) {
        qDebug() << "SetNicknameSuccess";
    } else {
        qWarning() << "Bind failed:" << retFullName.error().message();
        m_model->setResetUserNameError(retFullName.error().message());
    }
}

void SyncWorker::onSetFullname(const QString &fullname)
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
    QFuture<void> future = QtConcurrent::run(this, &SyncWorker::asyncSetFullname, fullname);
    watcher->setFuture(future);
}

void SyncWorker::onPullMessage()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    watcher->setFuture(QtConcurrent::run(this, &SyncWorker::futurePullMessage));
}

void SyncWorker::futurePullMessage()
{
    QDBusPendingReply<QString> retTime = DDBusSender()
                                         .service("com.deepin.sync.Daemon")
                                         .interface("com.deepin.utcloud.Daemon")
                                         .path("/com/deepin/utcloud/Daemon")
                                         .method("PullMessage")
                                         .call();
    if (!retTime.error().message().isEmpty())
        qDebug() << " message value: " << retTime.value();
}

void SyncWorker::getUserDeepinidInfo()
{
    getUOSID();
    getUUID();
    getHostName();
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
    m_syncInter->Logout();
    return result;
}

BindCheckResult SyncWorker::checkLocalBind(const QString &uuid)
{
    BindCheckResult result;
    QDBusPendingReply<QString> retLocalBindCheck = m_syncInter->LocalBindCheck(uuid);
    retLocalBindCheck.waitForFinished();
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
