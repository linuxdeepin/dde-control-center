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

    const QString activeApName() const { return m_activeAp; }

signals:
    void apAdded(const QJsonObject &apInfo) const;
    void apInfoChanged(const QJsonObject &apInfo) const;
    void apRemoved(const QString &ssid) const;
    void activeApChanged(const QString &oldName, const QString &newName) const;

public slots:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void deleteAP(const QString &apInfo);
    void setActiveApName(const QString &name);

private:
    QSet<QString> m_aps;
    QString m_activeAp;
};

}

}

#endif // WIRELESSDEVICE_H
