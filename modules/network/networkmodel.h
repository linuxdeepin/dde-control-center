#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"

#include <QObject>
#include <QMap>

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

    const QList<NetworkDevice *> devices() const { return m_devices; }
    const QString connectionUuid(const QString &connPath) const;

signals:
    void deviceListChanged(const QList<NetworkDevice *> devices) const;

private slots:
    void onDeviceListChanged(const QString &devices);
    void onConnectionListChanged(const QString &conns);
    void onConnectionSessionCreated(const QString &device, const QString &sessionPath);
    void onDeviceAPListChanged(const QString &device, const QString &apList);
    void onDeviceAPInfoChanged(const QString &device, const QString &apInfo);
    void onDeviceEnableChaned(const QString &device, const bool enabled);

private:
    bool containsDevice(const QString &devPath) const;
    NetworkDevice *device(const QString &devPath) const;

private:
    QList<NetworkDevice *> m_devices;
    QMap<QString, QString> m_connUuid;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODEL_H
