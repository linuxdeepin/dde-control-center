//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <DSysInfo>
#include <DPlatformHandle>

#include <QVariant>
#include <QSettings>

DCORE_USE_NAMESPACE

template <typename T>
T valueByQSettings(const QStringList& configFiles,
                   const QString&     group,
                   const QString&     key,
                   const QVariant&    failback)
{
    for (const QString& path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant& v = settings.value(key);
        if (v.isValid()) {
            T t = v.value<T>();
            return t;
        }
    }

    return failback.value<T>();
}

inline const static Dtk::Core::DSysInfo::UosType UosType = Dtk::Core::DSysInfo::uosType();
inline const static bool IsServerSystem =
        (Dtk::Core::DSysInfo::UosServer == UosType); // 是否是服务器版


#endif // UTILS_H
