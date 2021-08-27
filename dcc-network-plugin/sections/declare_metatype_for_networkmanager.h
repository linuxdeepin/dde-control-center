/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
