// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <DSysInfo>

DCORE_USE_NAMESPACE

#define DEEPINID_SERVICE   QStringLiteral("com.deepin.deepinid")
#define DEEPINID_PATH      QStringLiteral("/com/deepin/deepinid")
#define DEEPINID_INTERFACE QStringLiteral("com.deepin.deepinid")

#define SYNC_SERVICE   QStringLiteral("com.deepin.sync.Daemon")
#define SYNC_INTERFACE QStringLiteral("com.deepin.sync.Daemon")
#define SYNC_PATH      QStringLiteral("/com/deepin/sync/Daemon")

#define UTCLOUD_PATH      QStringLiteral("/com/deepin/utcloud/Daemon")
#define UTCLOUD_INTERFACE QStringLiteral("com.deepin.utcloud.Daemon")

#define LICENSE_SERVICE   QStringLiteral("com.deepin.license")
#define LICENSE_PATH      QStringLiteral("/com/deepin/license/Info")
#define LICENSE_INTERFACE QStringLiteral("com.deepin.license.Info")


const DSysInfo::UosType UosType = DSysInfo::uosType();
const DSysInfo::UosEdition UosEdition = DSysInfo::uosEditionType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType);          // 是否是服务器版
const bool IsCommunitySystem = (DSysInfo::UosCommunity == UosEdition); // 是否是社区版
const bool IsProfessionalSystem = (DSysInfo::UosProfessional == UosEdition); // 是否是专业版
const bool IsHomeSystem = (DSysInfo::UosHome == UosEdition);                 // 是否是个人版
const bool IsDeepinDesktop = (DSysInfo::DeepinDesktop == DSysInfo::deepinType()); // 是否是Deepin桌面


#endif // UTILS_H
