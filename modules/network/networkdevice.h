#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QObject>
#include <QJsonObject>
#include <QSet>

namespace dcc {

namespace network {

class NetworkDevice : public QObject
{
    Q_OBJECT

public:
    enum DeviceType
    {
        Wired,
        Wireless,
    };

public:
    explicit NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent = 0);
    explicit NetworkDevice(const NetworkDevice &device);

    bool enabled() const { return m_enabled; }
    void setEnabled(const bool enabled);
    DeviceType type() const { return m_type; }
    const QJsonObject info() const { return m_deviceInfo; }
    const QString path() const;

signals:
    void removed() const;
    void enableChanged(const bool enabled) const;
    void apAdded(const QJsonObject &apInfo);
    void apInfoChanged(const QJsonObject &apInfo);
    void apRemoved(const QString &ssid);

public slots:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void updateDeviceInfo(const QJsonObject &devInfo);

private:
    const DeviceType m_type;
    QJsonObject m_deviceInfo;

    bool m_enabled;

    QSet<QString> m_ssidSet;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKDEVICE_H
