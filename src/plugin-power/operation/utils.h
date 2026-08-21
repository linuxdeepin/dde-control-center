//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <DSysInfo>
#include <DPlatformHandle>

#include <QVariant>
#include <QSettings>
#include <QProcess>

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

static inline bool isServerSystem()
{
    static const bool serverSystem = Dtk::Core::DSysInfo::UosServer == Dtk::Core::DSysInfo::uosType();
    return serverSystem;
}

inline static bool isVirtualEnvironment()
{
    static bool cached = false;
    static bool result = false;
    
    if (!cached) {
        QProcess proc;
        proc.start("systemd-detect-virt");
        proc.waitForFinished(1000);
        QString output = proc.readAllStandardOutput();
        result = !output.contains("none");
        cached = true;
    }
    
    return result;
}

#endif // UTILS_H
