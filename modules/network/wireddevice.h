#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevice.h"

namespace dcc {

namespace network {

class WiredDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WiredDevice(const QJsonObject &info, QObject *parent = 0);
};

}

}

#endif // WIREDDEVICE_H
