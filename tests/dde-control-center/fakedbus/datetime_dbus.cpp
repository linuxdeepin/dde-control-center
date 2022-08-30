// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "datetime_dbus.h"

TimeDate::TimeDate(QObject *parent)
    : QObject(parent)
{
}

TimeDate::~TimeDate()
{
}

void TimeDate::AddUserTimezone(QString)
{
}

void TimeDate::DeleteUserTimezone(QString)
{
}

ZoneInfo TimeDate::GetZoneInfo(QString)
{
    ZoneInfo zone;
//    zone.m_zoneName = "Asia/Shanghai";
//    zone.m_zoneCity = "上海";
//    zone.m_utcOffset = 28800;
//    zone.i2 = 0;
//    zone.i3 = 0;
//    zone.i4 = 0;
    return zone;
}

QStringList TimeDate::GetZoneList()
{
    return QStringList();
}

QStringList TimeDate::GetSampleNTPServers()
{
    return QStringList();
}

void TimeDate::SetDate(int, int, int, int, int, int, int)
{
}

void TimeDate::SetLocalRTC(bool, bool)
{
}

void TimeDate::SetNTP(bool)
{
}

void TimeDate::SetNTPServer(const QString &)
{
}

void TimeDate::SetTime(qlonglong, bool)
{
}

void TimeDate::SetTimezone(const QString &)
{
}