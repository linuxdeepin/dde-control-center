#ifndef DECLARE_METATYPE_FOR_NETWORKMANAGER_H
#define DECLARE_METATYPE_FOR_NETWORKMANAGER_H

#include <networkmanagerqt/wirelesssecuritysetting.h>
#include <networkmanagerqt/security8021xsetting.h>

Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt);

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::EapMethod);

Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType);

#endif // DECLARE_METATYPE_FOR_NETWORKMANAGER_H
