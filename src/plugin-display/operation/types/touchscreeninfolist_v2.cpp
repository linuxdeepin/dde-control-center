//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

bool operator==(const TouchscreenInfo_V2 &info1, const TouchscreenInfo_V2 &info2) {
    return info1.id == info2.id && info1.name == info2.name && info1.deviceNode == info2.deviceNode && info1.serialNumber == info2.serialNumber && info1.UUID == info2.UUID;
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
