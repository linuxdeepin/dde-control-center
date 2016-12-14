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
        Unknow,
        Wired,
        Wireless,
    };

public:
    virtual ~NetworkDevice();

    bool enabled() const { return m_enabled; }
    void setEnabled(const bool enabled);
    DeviceType type() const { return m_type; }
    const QJsonObject info() const { return m_deviceInfo; }
    const QString path() const;

signals:
    void removed() const;
    void enableChanged(const bool enabled) const;

public slots:
    void updateDeviceInfo(const QJsonObject &devInfo);

protected:
    explicit NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent = 0);

private:
    const DeviceType m_type;
    QJsonObject m_deviceInfo;

    bool m_enabled;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKDEVICE_H
