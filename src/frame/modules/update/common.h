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

#include <QString>
#include <vector>
#include <qregexp.h>

using namespace std;


namespace dcc {
namespace update {

const double Epsion = 1e-6;
const QString SystemUpdateType = "system_upgrade";
const QString AppStoreUpdateType = "appstore_upgrade";
const QString SecurityUpdateType = "security_upgrade";
const QString UnknownUpdateType = "unknown_upgrade";

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
    NoNetwork,
    NoSpace,
    DeependenciesBrokenError,
    WaitRecoveryBackup,
    RecoveryBackingup,
    RecoveryBackingSuccessed,
    RecoveryBackupFailed,
    NoAtive
};

enum ShowStatus {
    NoActive,
    IsSuccessed,
    IsFailed
};

enum ClassifyUpdateType {
    Invalid = -1,
    SystemUpdate = 1,
    AppStoreUpdate,
    UnknownUpdate = 8,
    SecurityUpdate = 16
};

enum UpdateCtrlType {
    Start = 0,
    Pause
};

enum BackupStatus {
    NoBackup,
    Backingup,
    Backuped,
    BackupFailed
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

//equal : false
static inline bool compareDouble(const double value1, const double value2)
{
    return !((value1 - value2 >= -Epsion) && (value1 - value2 <= Epsion));
}

static inline QString formatCap(qulonglong cap, const int size = 1024)
{
    static QString type[] = {"B", "KB", "MB", "GB", "TB"};

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

static inline vector<double> getNumListFromStr(const QString& str)
{
    //筛选出字符串中的数字
    QRegExp rx("-?[1-9]\\d*\\.\\d*|0+.[0-9]+|-?0\\.\\d*[1-9]\\d*|-?\\d+");
    int pos = 0;
    vector<double> v;
    while ((pos = rx.indexIn(str, pos)) != -1)
    {
        pos += rx.matchedLength();
        v.push_back(rx.cap(0).toDouble());
    }
    return v;
}

}
}

#endif // COMMON_H
