#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevice.h"

#include <QMap>

namespace dcc {

namespace network {

class WirelessDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = 0);

signals:
    void apAdded(const QJsonObject &apInfo) const;
    void apInfoChanged(const QJsonObject &apInfo) const;
    void apRemoved(const QString &ssid) const;

public slots:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void deleteAP(const QString &apInfo);

private:
    QSet<QString> m_aps;
};

}

}

#endif // WIRELESSDEVICE_H
