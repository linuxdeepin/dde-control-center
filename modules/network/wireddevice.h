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

public slots:
    void onConnectionInfoChanged(const QJsonObject &connInfo);

private:
    QJsonObject m_wiredConnection;
};

}

}

#endif // WIREDDEVICE_H
