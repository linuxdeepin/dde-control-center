//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "zoneinfo.h"

ZoneInfo::ZoneInfo()
    : m_utcOffset(0)
    , i2(0)
    , i3(0)
    , i4(0)
{
}

QDebug operator<<(QDebug argument, const ZoneInfo &info)
{
    argument << QString("ZoneInfo(");
    argument << info.m_zoneName << "," << info.m_zoneCity << "," << info.m_utcOffset;
    argument << ",(";
    argument << info.i2 << "," << info.i3 << "," << info.i4;
    argument << "))";
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const ZoneInfo &info)
{
    argument.beginStructure();
    argument << info.m_zoneName << info.m_zoneCity << info.m_utcOffset;
    argument.beginStructure();
    argument << info.i2 << info.i3 << info.i4;
    argument.endStructure();
    argument.endStructure();
    return argument;
}

QDataStream &operator<<(QDataStream &argument, const ZoneInfo &info)
{
    argument << info.m_zoneName << info.m_zoneCity << info.m_utcOffset;
    argument << info.i2 << info.i3 << info.i4;
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ZoneInfo &info)
{
    argument.beginStructure();
    argument >> info.m_zoneName >> info.m_zoneCity >> info.m_utcOffset;
    argument.beginStructure();
    argument >> info.i2 >> info.i3 >> info.i4;
    argument.endStructure();
    argument.endStructure();
    return argument;
}

const QDataStream &operator>>(QDataStream &argument, ZoneInfo &info)
{
    argument >> info.m_zoneName >> info.m_zoneCity >> info.m_utcOffset;
    argument >> info.i2 >> info.i3 >> info.i4;
    return argument;
}

bool ZoneInfo::operator==(const ZoneInfo &what) const
{
    return m_zoneName == what.m_zoneName
            && m_zoneCity == what.m_zoneCity
            && m_utcOffset == what.m_utcOffset
            && i2 == what.i2
            && i3 == what.i3
            && i4 == what.i4;
}

void registerZoneInfoMetaType()
{
    qRegisterMetaType<ZoneInfo>("ZoneInfo");
    qDBusRegisterMetaType<ZoneInfo>();
}
