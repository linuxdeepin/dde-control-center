/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "datetimemodel.h"
#include "window/utils.h"

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>

namespace dcc {
namespace datetime {

DatetimeModel::DatetimeModel(QObject *parent)
    : QObject(parent)
    , m_ntp(true)
    , m_bUse24HourType(true)
{
    m_bSystemIsServer = DCC_NAMESPACE::IsServerSystem;
}

void DatetimeModel::setNTP(bool ntp)
{
    if (m_ntp != ntp) {
        m_ntp = ntp;
        Q_EMIT NTPChanged(ntp);
    }
}

void DatetimeModel::set24HourFormat(bool state)
{
    if (m_bUse24HourType != state) {
        m_bUse24HourType = state;
        Q_EMIT hourTypeChanged(state);
    }
}

#ifndef DCC_DISABLE_TIMEZONE
QString DatetimeModel::systemTimeZoneId() const
{
    return m_systemTimeZoneId;
}

void DatetimeModel::setSystemTimeZoneId(const QString &systemTimeZoneId)
{
    if (m_systemTimeZoneId != systemTimeZoneId) {
        m_systemTimeZoneId = systemTimeZoneId;
        Q_EMIT systemTimeZoneIdChanged(systemTimeZoneId);
    }
}
#endif

QList<ZoneInfo> DatetimeModel::userTimeZones() const
{
    return m_userTimeZones;
}

void DatetimeModel::addUserTimeZone(const ZoneInfo &zone)
{
    const QString zoneName = zone.getZoneName();

    if (!m_userZoneIds.contains(zoneName)) {
        m_userZoneIds.append(zoneName);
        m_userTimeZones.append(zone);
        Q_EMIT userTimeZoneAdded(zone);
    }
}

void DatetimeModel::removeUserTimeZone(const ZoneInfo &zone)
{
    const QString zoneName = zone.getZoneName();

    if (m_userZoneIds.contains(zoneName)) {
        m_userZoneIds.removeAll(zoneName);
        m_userTimeZones.removeAll(zone);
        Q_EMIT userTimeZoneRemoved(zone);
    }
}

void DatetimeModel::setCurrentTimeZone(const ZoneInfo &currentTimeZone)
{
    if (m_currentTimeZone == currentTimeZone)
        return;

    m_currentTimeZone = currentTimeZone;

    Q_EMIT currentTimeZoneChanged(currentTimeZone);
}

void DatetimeModel::setCurrentUseTimeZone(const ZoneInfo &currentSysTimeZone)
{
    if (m_currentSystemTimeZone == currentSysTimeZone)
        return;

    m_currentSystemTimeZone = currentSysTimeZone;

    Q_EMIT currentSystemTimeZoneChanged(currentSysTimeZone);
}

void DatetimeModel::setNtpServerAddress(const QString &ntpServer)
{
    if (m_bSystemIsServer && m_strNtpServerAddress != ntpServer) {
        m_strNtpServerAddress = ntpServer;
        Q_EMIT NTPServerChanged(ntpServer);
    }
}

void DatetimeModel::setNTPServerList(const QStringList &list)
{
    if (m_bSystemIsServer && m_NtpServerList != list) {
        m_NtpServerList = list;
        Q_EMIT NTPServerListChanged(list);
    }
}

void DatetimeModel::setTimeZoneInfo(const QString &timeZone)
{
    if (m_timeZones != timeZone) {
        m_timeZones = timeZone;
        Q_EMIT timeZoneChanged(timeZone);
    }
}

void DatetimeModel::setWeekdayFormatType(int type)
{
    if (m_weekdayFormatType == type) return;

    m_weekdayFormatType = type;
    Q_EMIT weekdayFormatTypeChanged(type);
}

void DatetimeModel::setShortDateFormat(int type)
{
    if (m_shortDateFormat == type) return;

    m_shortDateFormat = type;
    Q_EMIT shortDateFormatChanged(type);
}

void DatetimeModel::setLongDateFormat(int type)
{
    if (m_longDateFormat == type) return;

    m_longDateFormat = type;
    Q_EMIT longDateFormatChanged(type);
}

void DatetimeModel::setLongTimeFormat(int type)
{
    if (m_longTimeFormat == type) return;

    m_longTimeFormat = type;
    Q_EMIT longTimeFormatChanged(type);
}

void DatetimeModel::setShorTimeFormat(int type)
{
    if (m_shorTimeFormat == type) return;

    m_shorTimeFormat = type;
    Q_EMIT shorTimeFormatChanged(type);
}

void DatetimeModel::setWeekStartDayFormat(int type)
{
    if (m_weekStartDayFormat == type) return;

    m_weekStartDayFormat = type;
    Q_EMIT weekStartDayFormatChanged(type);
}

void DatetimeModel::setWeekdayFormatTypeCount(int size)
{
    if (m_weekdayFormatTypeCount == size) return;

    m_weekdayFormatTypeCount = size;
}

void DatetimeModel::setShortDateFormatTypeCount(int size)
{
    if (m_shortDateFormatTypeCount == size) return;

    m_shortDateFormatTypeCount = size;
}

void DatetimeModel::setLongDateFormatTypeCount(int size)
{
    if (m_longDateFormatTypeCount == size) return;

    m_longDateFormatTypeCount = size;
}

void DatetimeModel::setLongTimeFormatTypeCount(int size)
{
    if (m_longTimeFormatTypeCount == size) return;

    m_longTimeFormatTypeCount = size;
}

void DatetimeModel::setShorTimeFormatTypeCount(int size)
{
    if (m_shorTimeFormatTypeCount == size) return;

    m_shorTimeFormatTypeCount = size;
}

void DatetimeModel::setWeekStartDayFormatTypeCount(int size)
{
    if (m_weekStartDayFormatTypeCount == size) return;

    m_weekStartDayFormatTypeCount = size;
}
}
}
