#ifndef UTILS_H
#define UTILS_H

#include "networkconst.h"

namespace dde {
namespace network {

#define PRINT_INFO_MESSAGE(msg) qInfo() << __FILE__ << "line:" << __LINE__ << "function:" << __FUNCTION__ << "Message:" << msg
#define PRINT_DEBUG_MESSAGE(msg) qDebug() << __FILE__ << "line:" << __LINE__ << "function:" << __FUNCTION__ << "Message:" << msg

// an alias for numeric zero, no flags set.
#define DEVICE_INTERFACE_FLAG_NONE 0
// the interface is enabled from the administrative point of view. Corresponds to kernel IFF_UP.
#define DEVICE_INTERFACE_FLAG_UP 0x1
// the physical link is up. Corresponds to kernel IFF_LOWER_UP.
#define DEVICE_INTERFACE_FLAG_LOWER_UP 0x2
// the interface has carrier. In most cases this is equal to the value of @NM_DEVICE_INTERFACE_FLAG_LOWER_UP
#define DEVICE_INTERFACE_FLAG_CARRIER 0x10000

Connectivity connectivityValue(uint sourceConnectivity);
DeviceStatus convertDeviceStatus(int sourceDeviceStatus);
ConnectionStatus convertConnectionStatus(int sourceConnectionStatus);

}
}
#endif // UTILS_H
