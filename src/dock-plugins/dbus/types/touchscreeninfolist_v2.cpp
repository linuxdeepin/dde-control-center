// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "touchscreeninfolist_v2.h"

QDBusArgument &operator<<(QDBusArgument &arg, const TouchscreenInfo_V2 &info)
{
    arg.beginStructure();
    arg << info.id << info.name << info.deviceNode << info.serialNumber << info.UUID;
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, TouchscreenInfo_V2 &info)
{
    arg.beginStructure();
    arg >> info.id >> info.name >> info.deviceNode >> info.serialNumber >> info.UUID;
    arg.endStructure();

    return arg;
}

bool TouchscreenInfo_V2::operator==(const TouchscreenInfo_V2 &info) const
{
    return id == info.id && name == info.name && deviceNode == info.deviceNode && serialNumber == info.serialNumber && UUID == info.UUID;
}

bool TouchscreenInfo_V2::operator!=(const TouchscreenInfo_V2 &info) const
{
    return !(*this == info);
}

void registerTouchscreenInfoV2MetaType()
{
    qRegisterMetaType<TouchscreenInfo_V2>("TouchscreenInfo_V2");
    qDBusRegisterMetaType<TouchscreenInfo_V2>();
}

void registerTouchscreenInfoList_V2MetaType()
{
    registerTouchscreenInfoV2MetaType();

    qRegisterMetaType<TouchscreenInfoList_V2>("TouchscreenInfoList_V2");
    qDBusRegisterMetaType<TouchscreenInfoList_V2>();
}
