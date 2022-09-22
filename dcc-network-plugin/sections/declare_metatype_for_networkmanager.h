// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DECLARE_METATYPE_FOR_NETWORKMANAGER_H
#define DECLARE_METATYPE_FOR_NETWORKMANAGER_H

#include <networkmanagerqt/wirelesssecuritysetting.h>
#include <networkmanagerqt/security8021xsetting.h>

Q_DECLARE_METATYPE(WirelessSecuritySetting::KeyMgmt)

Q_DECLARE_METATYPE(Security8021xSetting::EapMethod)

Q_DECLARE_METATYPE(Setting::SecretFlagType)

Q_DECLARE_METATYPE(Security8021xSetting::FastProvisioning)

Q_DECLARE_METATYPE(Security8021xSetting::AuthMethod)

Q_DECLARE_METATYPE(Security8021xSetting::PeapVersion)

#endif // DECLARE_METATYPE_FOR_NETWORKMANAGER_H
