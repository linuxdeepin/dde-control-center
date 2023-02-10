/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef COMMON_H
#define COMMON_H

#include <qregexp.h>

#include <DSysInfo>

#include <QString>

#include <vector>

DCORE_USE_NAMESPACE

using namespace std;

const double Epsion = 1e-6;
const QString SystemUpdateType = "system_upgrade";
const QString AppStoreUpdateType = "appstore_upgrade";
const QString SecurityUpdateType = "security_upgrade";
const QString UnknownUpdateType = "unknown_upgrade";

const DSysInfo::UosType UosType = DSysInfo::uosType();
const DSysInfo::UosEdition UosEdition = DSysInfo::uosEditionType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType);          // 是否是服务器版
const bool IsCommunitySystem = (DSysInfo::UosCommunity == UosEdition); // 是否是社区版
const bool IsProfessionalSystem = (DSysInfo::UosProfessional == UosEdition); // 是否是专业版
const bool IsHomeSystem = (DSysInfo::UosHome == UosEdition);                 // 是否是个人版
const bool IsEducationSystem = (DSysInfo::UosEducation == UosEdition);       // 是否是教育版
const bool IsDeepinDesktop = (DSysInfo::DeepinDesktop == DSysInfo::deepinType()); // 是否是Deepin桌面

enum UpdatesStatus {
    Default,
    Checking,
    Updated,
    UpdatesAvailable,
    Updateing,
    Downloading,
    DownloadPaused,
    Downloaded,
    AutoDownloaded,
    Installing,
    UpdateSucceeded,
    UpdateFailed,
    NeedRestart,
    WaitRecoveryBackup,
    RecoveryBackingup,
    RecoveryBackingSuccessed,
    RecoveryBackupFailed,
    NoAtive
};

enum UpdateErrorType {
    NoError,
    NoNetwork,
    NoSpace,
    DeependenciesBrokenError,
    DpkgInterrupted,
    UnKnown
};

enum ShowStatus { NoActive, IsSuccessed, IsFailed };

enum ClassifyUpdateType {
    Invalid = -1,
    SystemUpdate = 1,
    AppStoreUpdate,
    UnknownUpdate = 8,
    SecurityUpdate = 16
};

enum UpdateCtrlType { Start = 0, Pause };

// 备份状态
enum BackupStatus { NoBackup, Backingup, Backuped, BackupFailed };

// 原子更新结果 后期更具需求拓展
enum BackupResult { Success = 0, BackingUp = 1 };

enum UiActiveState {
    Unknown = -1,     // 未知
    Unauthorized = 0, // 未授权
    Authorized,       // 已授权
    AuthorizedLapse,  // 授权失效
    TrialAuthorized,  // 试用期已授权
    TrialExpired      // 试用期已过期
};

static inline ClassifyUpdateType uintToclassifyUpdateType(uint type)
{
    ClassifyUpdateType value = ClassifyUpdateType::Invalid;
    switch (type) {
    case ClassifyUpdateType::SystemUpdate:
        value = ClassifyUpdateType::SystemUpdate;
        break;
    case ClassifyUpdateType::SecurityUpdate:
        value = ClassifyUpdateType::SecurityUpdate;
        break;
    case ClassifyUpdateType::UnknownUpdate:
        value = ClassifyUpdateType::UnknownUpdate;
        break;
    default:
        value = ClassifyUpdateType::Invalid;
        break;
    }

    return value;
}

// equal : false
static inline bool compareDouble(const double value1, const double value2)
{
    return !((value1 - value2 >= -Epsion) && (value1 - value2 <= Epsion));
}

static inline QString formatCap(qulonglong cap, const int size = 1024)
{
    static QString type[] = { "B", "KB", "MB", "GB", "TB" };

    if (cap < qulonglong(size)) {
        return QString::number(cap) + type[0];
    }
    if (cap < qulonglong(size) * size) {
        return QString::number(double(cap) / size, 'f', 2) + type[1];
    }
    if (cap < qulonglong(size) * size * size) {
        return QString::number(double(cap) / size / size, 'f', 2) + type[2];
    }
    if (cap < qulonglong(size) * size * size * size) {
        return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];
    }

    return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
}

static inline vector<double> getNumListFromStr(const QString &str)
{
    // 筛选出字符串中的数字
    QRegExp rx("-?[1-9]\\d*\\.\\d*|0+.[0-9]+|-?0\\.\\d*[1-9]\\d*|-?\\d+");
    int pos = 0;
    vector<double> v;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        pos += rx.matchedLength();
        v.push_back(rx.cap(0).toDouble());
    }
    return v;
}

#endif // COMMON_H
