// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include <QDBusMetaType>
#include <QDebug>

class DMIInfo
{
public:
    DMIInfo();

    friend QDebug operator<<(QDebug debug, const DMIInfo &info);
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, DMIInfo &info);
    friend QDBusArgument &operator<<(QDBusArgument &arg, const DMIInfo &info);

public:
    QString biosVendor{""};
    QString biosVersion{""};
    QString biosDate{""};
    QString boardName{""};
    QString boardSerial{""};
    QString boardVendor{""};
    QString boardVersion{""};
    QString productName{""};
    QString productFamily{""};
    QString producctSerial{""};
    QString productUUID{""};
    QString productVersion{""};
};

Q_DECLARE_METATYPE(DMIInfo)

void registerDMIInfoMetaType();

class HardwareInfo
{
public:
    HardwareInfo();

    friend QDebug operator<<(QDebug debug, const HardwareInfo &info);
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, HardwareInfo &info);
    friend QDBusArgument &operator<<(QDBusArgument &arg, const HardwareInfo &info);

public:
    QString id{""};
    QString hostName{""};
    QString username{""};
    QString os{""};
    QString cpu{""};
    bool laptop{false};
    qint64 memory{0};
    qint64 diskTotal{0};
    QString networkCards{""};
    QString diskList{""};
    DMIInfo dmi;
};

Q_DECLARE_METATYPE(HardwareInfo)

void registerHardwareInfoMetaType();


#endif // HARDWAREINFO_H
