#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevice.h"

namespace dcc {

namespace network {

class WirelessDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = 0);

signals:
    void apAdded(const QJsonObject &apInfo);
    void apInfoChanged(const QJsonObject &apInfo);
    void apRemoved(const QString &ssid);

public slots:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);

private:
    QSet<QString> m_ssidSet;
};

}

}

#endif // WIRELESSDEVICE_H
