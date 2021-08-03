/*
 * Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     dr <dongrui@uniontech.com>
 *
 * Maintainer: dr <dongrui@uniontech.com>
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
