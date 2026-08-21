// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <DSysInfo>

DCORE_USE_NAMESPACE

// com.deepin.deepinid 主要是用于登陆登出，和账户信息相关
#define DEEPINID_SERVICE   QStringLiteral("com.deepin.deepinid")
#define DEEPINID_PATH      QStringLiteral("/com/deepin/deepinid")
#define DEEPINID_INTERFACE QStringLiteral("com.deepin.deepinid")

// com.deepin.sync.Daemon 主要是用于dde系统设置同步
#define SYNC_SERVICE   QStringLiteral("com.deepin.sync.Daemon")
#define SYNC_INTERFACE QStringLiteral("com.deepin.sync.Daemon")
#define SYNC_PATH      QStringLiteral("/com/deepin/sync/Daemon")

// com.deepin.sync.Daemon 主要是用于应用数据同步（例如：日历、浏览器...）
#define UTCLOUD_PATH      QStringLiteral("/com/deepin/utcloud/Daemon")
#define UTCLOUD_INTERFACE QStringLiteral("com.deepin.utcloud.Daemon")

#define LICENSE_SERVICE   QStringLiteral("com.deepin.license")
#define LICENSE_PATH      QStringLiteral("/com/deepin/license/Info")
#define LICENSE_INTERFACE QStringLiteral("com.deepin.license.Info")

#define DEEPINCLIENT_SERVICE QStringLiteral("com.deepin.deepinid.Client")
#define DEEPINCLIENT_PATH QStringLiteral("/com/deepin/deepinid/Client")
#define DEEPINCLIENT_INTERFACE QStringLiteral("com.deepin.deepinid.Client")

static inline bool isServerSystem()
{
    static const bool serverSystem = DSysInfo::UosServer == DSysInfo::uosType();
    return serverSystem;
}

static inline bool isCommunitySystem()
{
    static const bool communitySystem = DSysInfo::UosCommunity == DSysInfo::uosEditionType();
    return communitySystem;
}

static inline bool isProfessionalSystem()
{
    static const bool professionalSystem = DSysInfo::UosProfessional == DSysInfo::uosEditionType();
    return professionalSystem;
}

static inline bool isHomeSystem()
{
    static const bool homeSystem = DSysInfo::UosHome == DSysInfo::uosEditionType();
    return homeSystem;
}

static inline bool isDeepinDesktop()
{
    static const bool deepinDesktop = DSysInfo::DeepinDesktop == DSysInfo::deepinType();
    return deepinDesktop;
}

enum SyncType {
    Sound,
    Theme,
    Power,
    Network,
    Mouse,
    Update,
    Dock,
    Launcher,
    Wallpaper,
    Screensaver
};


namespace utils {

QString getUrlTitle();

QString forgetPwdURL();

QString wechatURL();

QString getThemeName();

QString getActiveColor();

QString getStandardFont();

QString getDeviceKernel();

QString getOsVersion();

QString getDeviceCode();

QStringList getDeviceInfo();

QString getEditionName();

QString getIconName();

} // namespace utils


#endif // UTILS_H
