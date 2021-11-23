#ifndef UTILS_H
#define UTILS_H

#include "networkconst.h"

using namespace dde::network;

#define PRINT_INFO_MESSAGE(msg) qInfo() << __FILE__ << "line:" << __LINE__ << "function:" << __FUNCTION__ << "Message:" << msg
#define PRINT_DEBUG_MESSAGE(msg) qDebug() << __FILE__ << "line:" << __LINE__ << "function:" << __FUNCTION__ << "Message:" << msg

Connectivity connectivityValue(uint sourceConnectivity);
DeviceStatus convertDeviceStatus(int sourceDeviceStatus);
ConnectionStatus convertConnectionStatus(int sourceConnectionStatus);

#endif // UTILS_H
