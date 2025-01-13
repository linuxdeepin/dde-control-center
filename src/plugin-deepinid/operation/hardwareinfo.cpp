// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hardwareinfo.h"

DMIInfo::DMIInfo()
{
}

void registerDMIInfoMetaType()
{
    qRegisterMetaType<DMIInfo>("DMIInfo");
    qDBusRegisterMetaType<DMIInfo>();
}

QDebug operator<<(QDebug debug, const DMIInfo &info)
{
    debug << QString("DMIInfo(") << info.biosVendor << ", " << info.biosVersion << ", "
          << info.biosDate << ", " << info.boardName << ", " << info.boardSerial << ", "
          << info.boardVendor << ", " << info.boardVersion << ", "
          << info.productName << ", " << info.productFamily << ", "
          << info.producctSerial << ", " << info.productUUID << ", "
          << info.productVersion << ")";

    return debug;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DMIInfo &info)
{
    arg.beginStructure();
    arg >> info.biosVendor >> info.biosVersion >> info.biosDate
            >> info.boardName >> info.boardSerial >> info.boardVendor >> info.boardVersion
            >> info.productName >> info.productFamily
            >> info.producctSerial >> info.productUUID >> info.productVersion;
    arg.endStructure();

    return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const DMIInfo &info)
{
    arg.beginStructure();
    arg << info.biosVendor << info.biosVersion << info.biosDate
            << info.boardName << info.boardSerial << info.boardVendor << info.boardVersion
            << info.productName << info.productFamily
            << info.producctSerial << info.productUUID << info.productVersion;
    arg.endStructure();

    return arg;
}

HardwareInfo::HardwareInfo()
{

}

QDebug operator<<(QDebug debug, const HardwareInfo &info)
{
    debug << "HardwareInfo(" <<info.id << ", " << info.hostName  << ", "
          << info.username  << ", " << info.os  << ", " << info.cpu << ", "
          << info.laptop  << ", " << info.memory  << ", " << info.diskTotal
          << ", " << info.networkCards << ", " << info.diskList  << ", " << info.dmi << ')';

    return debug;
}

QDBusArgument &operator<<(QDBusArgument &arg, const HardwareInfo &info)
{
    arg.beginStructure();
    arg << info.id << info.hostName << info.username << info.os << info.cpu
            << info.laptop << info.memory << info.diskTotal << info.networkCards
            << info.diskList << info.dmi;
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, HardwareInfo &info)
{
    arg.beginStructure();
    arg >> info.id >> info.hostName >> info.username >> info.os >> info.cpu
            >> info.laptop >> info.memory >> info.diskTotal >> info.networkCards
            >> info.diskList >> info.dmi;
    arg.endStructure();

    return arg;
}


void registerHardwareInfoMetaType()
{
    qRegisterMetaType<HardwareInfo>("HardwareInfo");
    qDBusRegisterMetaType<HardwareInfo>();
}
