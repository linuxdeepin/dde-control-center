#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"

#include <QObject>

namespace dcc {

namespace network {

class NetworkDevice;
class NetworkWorker;
class NetworkModel : public QObject
{
    Q_OBJECT

    friend class NetworkWorker;

public:
    explicit NetworkModel(QObject *parent = 0);
    ~NetworkModel();

    const QList<NetworkDevice *> wiredDeviceList() const { return m_wiredDevices; }
    const QList<NetworkDevice *> wirelessDeviceList() const { return m_wirelessDevices; }

signals:
    void wiredDeviceListChanged(const QList<NetworkDevice *>) const;
    void wirelessDeviceListChanged(const QList<NetworkDevice *>) const;

private slots:
    void onDevicesPropertyChanged(const QString &devices);

private:
    void updateDeviceList(const NetworkDevice::DeviceType type, const QJsonArray &list);

private:
    QList<NetworkDevice *> m_wiredDevices;
    QList<NetworkDevice *> m_wirelessDevices;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODEL_H
