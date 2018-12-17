#include "syncworker.h"

using namespace dcc;
using namespace dcc::sync;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";

static const QMap<SyncModel::SyncType, QString> SYNC_TYPE_MAP {
    {SyncModel::SyncType::Network, "network"},
    {SyncModel::SyncType::Sound, "sound_effect"},
    {SyncModel::SyncType::Mouse, "peripherals"},
    {SyncModel::SyncType::Update, "updater"},
    {SyncModel::SyncType::Dock, "dock"},
    {SyncModel::SyncType::Launcher, "launcher"},
    {SyncModel::SyncType::Wallpaper, "Background"},
};

SyncWorker::SyncWorker(SyncModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
{
    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);
}

void SyncWorker::activate()
{

}

void SyncWorker::deactivate()
{

}

void SyncWorker::setSync(SyncModel::SyncType type, bool enable)
{
    m_syncInter->SwitcherSet(SYNC_TYPE_MAP[type], enable);
}
