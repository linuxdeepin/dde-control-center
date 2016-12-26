#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevice.h"

#include <QJsonObject>

namespace dcc {

namespace network {

class WiredDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WiredDevice(const QJsonObject &info, QObject *parent = 0);

    const QJsonObject connection() const { return m_wiredConnection; }
    const QList<QJsonObject> pppConnections() const { return m_pppConnections; }

signals:
    void pppInfoListChanged(const QList<QJsonObject> &pppInfoList) const;

public slots:
    void onConnectionInfoChanged(const QJsonObject &connInfo);
    void onPPPInfoChanged(QList<QJsonObject> &pppInfoList);

private:
    QJsonObject m_wiredConnection;
    QList<QJsonObject> m_pppConnections;
};

}

}

#endif // WIREDDEVICE_H
