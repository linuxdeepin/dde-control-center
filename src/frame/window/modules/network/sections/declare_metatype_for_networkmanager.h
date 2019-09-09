#ifndef DECLARE_METATYPE_FOR_NETWORKMANAGER_H
#define DECLARE_METATYPE_FOR_NETWORKMANAGER_H

#include <networkmanagerqt/wirelesssecuritysetting.h>
#include <networkmanagerqt/security8021xsetting.h>

Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt)

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::EapMethod)

Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType)

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::FastProvisioning)

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::AuthMethod)

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::PeapVersion)

#endif // DECLARE_METATYPE_FOR_NETWORKMANAGER_H
