#include "networkworker.h"
#include "networkmodel.h"

using namespace dcc::network;

NetworkWorker::NetworkWorker(NetworkModel *model, QObject *parent)
    : QObject(parent),

      m_networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this),

      m_networkModel(model)
{
    connect(&m_networkInter, &NetworkInter::DevicesChanged, m_networkModel, &NetworkModel::onDevicesPropertyChanged);
    connect(&m_networkInter, &NetworkInter::DeviceEnabled, m_networkModel, &NetworkModel::onDeviceEnableChaned);

    m_networkModel->onDevicesPropertyChanged(m_networkInter.devices());
}

void NetworkWorker::setDeviceEnable(const QString &devPath, const bool enable)
{
    m_networkInter.EnableDevice(QDBusObjectPath(devPath), enable);
}
