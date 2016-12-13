#include "wireddevice.h"

using namespace dcc::network;

WiredDevice::WiredDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wired, info, parent)
{

}
