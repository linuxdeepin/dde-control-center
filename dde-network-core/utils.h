#ifndef UTILS_H
#define UTILS_H

#include "networkconst.h"

using namespace dde::network;

Connectivity connectivityValue(uint sourceConnectivity);
DeviceStatus convertDeviceStatus(int sourceDeviceStatus);
ConnectionStatus convertConnectionStatus(int sourceConnectionStatus);

#endif // UTILS_H
