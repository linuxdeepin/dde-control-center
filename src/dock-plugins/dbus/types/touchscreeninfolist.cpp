// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "touchscreeninfolist.h"

QDBusArgument &operator<<(QDBusArgument &arg, const TouchscreenInfo &info)
{
    arg.beginStructure();
    arg << info.id << info.name << info.deviceNode << info.serialNumber;
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, TouchscreenInfo &info)
{
    arg.beginStructure();
    arg >> info.id >> info.name >> info.deviceNode >> info.serialNumber;
    arg.endStructure();

    return arg;
}

bool TouchscreenInfo::operator==(const TouchscreenInfo &info) const
{
    return id == info.id && name == info.name && deviceNode == info.deviceNode && serialNumber == info.serialNumber;
}

bool TouchscreenInfo::operator!=(const TouchscreenInfo &info) const
{
    return !(*this == info);
}

void registerTouchscreenInfoMetaType()
{
    qRegisterMetaType<TouchscreenInfo>("TouchscreenInfo");
    qDBusRegisterMetaType<TouchscreenInfo>();
}

void registerTouchscreenInfoListMetaType()
{
    registerTouchscreenInfoMetaType();

    qRegisterMetaType<TouchscreenInfoList>("TouchscreenInfoList");
    qDBusRegisterMetaType<TouchscreenInfoList>();
}
