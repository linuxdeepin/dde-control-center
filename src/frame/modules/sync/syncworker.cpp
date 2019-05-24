#include "syncworker.h"

using namespace dcc;
using namespace dcc::sync;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";

SyncWorker::SyncWorker(SyncModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
{
    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);

    connect(m_deepinId_inter, &DeepinId::UserInfoChanged, m_model, &SyncModel::setUserinfo);
    connect(m_syncInter, &SyncInter::StateChanged, this, &SyncWorker::onStateChanged);
    connect(m_syncInter, &SyncInter::LastSyncTimeChanged, m_model, &SyncModel::setLastSyncTime);
    connect(m_syncInter, &SyncInter::SwitcherChange, this, &SyncWorker::onSyncModuleStateChanged);
}

void SyncWorker::activate()
{
    m_syncInter->blockSignals(false);
    m_deepinId_inter->blockSignals(false);

    m_model->setUserinfo(m_deepinId_inter->userInfo());
    onStateChanged(m_syncInter->state());
    m_model->setLastSyncTime(m_syncInter->lastSyncTime());

    refreshSyncState();
}

void SyncWorker::deactivate()
{
    m_syncInter->blockSignals(true);
    m_deepinId_inter->blockSignals(true);
}

void SyncWorker::refreshSyncState()
{
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(m_syncInter->SwitcherDump(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &SyncWorker::onGetModuleSyncStateFinished);
}

void SyncWorker::setSync(std::pair<SyncModel::SyncType, bool> state)
{
    m_syncInter->SwitcherSet(m_model->moduleMap()[state.first], state.second);
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

void SyncWorker::onSyncModuleStateChanged(const QString& module, bool enable) {
    if (module == "enabled") {
        return m_model->setEnableSync(enable);
    }

    m_model->setModuleSyncState(m_model->moduleMap().key(module), enable);
}

void SyncWorker::onStateChanged(const IntString &state)
{
    m_model->setSyncState(std::pair<qint32, QString>(state.state, state.description));
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

    QMap<SyncModel::SyncType, QString> moduleMap = m_model->moduleMap();
    for (auto it = moduleMap.cbegin(); it != moduleMap.cend(); ++it) {
        m_model->setModuleSyncState(it.key(), obj[it.value()].toBool());
    }
}
