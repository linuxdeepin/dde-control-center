#include "netutils.h"

namespace dde {
namespace network {

Connectivity connectivityValue(uint sourceConnectivity)
{
    switch (sourceConnectivity) {
    case 0:     return Connectivity::Unknownconnectivity;
    case 1:     return Connectivity::Noconnectivity;
    case 2:     return Connectivity::Portal;
    case 3:     return Connectivity::Limited;
    case 4:     return Connectivity::Full;
    default:    break;
    }

    return Connectivity::Unknownconnectivity;
}

DeviceStatus convertDeviceStatus(int sourceDeviceStatus)
{
    switch (sourceDeviceStatus) {
    case 0:     return DeviceStatus::Unknown;
    case 10:    return DeviceStatus::Unmanaged;
    case 20:    return DeviceStatus::Unavailable;
    case 30:    return DeviceStatus::Disconnected;
    case 40:    return DeviceStatus::Prepare;
    case 50:    return DeviceStatus::Config;
    case 60:    return DeviceStatus::Needauth;
    case 70:    return DeviceStatus::IpConfig;
    case 80:    return DeviceStatus::IpCheck;
    case 90:    return DeviceStatus::Secondaries;
    case 100:   return DeviceStatus::Activated;
    case 110:   return DeviceStatus::Deactivation;
    case 120:   return DeviceStatus::Failed;
    default:    break;
    }

    return DeviceStatus::Unknown;
}

ConnectionStatus convertConnectionStatus(int sourceConnectionStatus)
{
    switch (sourceConnectionStatus) {
    case 0:     return ConnectionStatus::Unknown;
    case 1:     return ConnectionStatus::Activating;
    case 2:     return ConnectionStatus::Activated;
    case 3:     return ConnectionStatus::Deactivating;
    case 4:     return ConnectionStatus::Deactivated;
    }

    return ConnectionStatus::Unknown;
}

}
}
