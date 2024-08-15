//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimemodel.h"

#include <QDateTime>
#include <QTimeZone>
#include <QSettings>

DatetimeModel::DatetimeModel(QObject *parent)
    : QObject(parent)
    , m_ntp(true)
    , m_bUse24HourType(true)
{
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
    if (m_strNtpServerAddress != ntpServer) {
        m_strNtpServerAddress = ntpServer;
        Q_EMIT NTPServerChanged(ntpServer);
    }
}

void DatetimeModel::setNTPServerList(const QStringList &list)
{
    if (m_NtpServerList != list) {
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

void DatetimeModel::setCountry(const QString &country)
{
    if (m_country != country) {
        m_country = country;
        Q_EMIT countryChanged(country);
    }
}

void DatetimeModel::setLocaleName(const QString &localeName)
{
    if (m_localeName != localeName) {
        m_localeName = localeName;
        Q_EMIT localeNameChanged(localeName);
    }
}

void DatetimeModel::setLangRegion(const QString &langCountry)
{
    if (m_langCountry != langCountry) {
        m_langCountry = langCountry;
        Q_EMIT langCountryChanged(langCountry);
    }
}

void DatetimeModel::setFirstDayOfWeek(const int &firstDayOfWeekFormat)
{
    if (m_firstDayOfWeekFormat != firstDayOfWeekFormat) {
        m_firstDayOfWeekFormat = firstDayOfWeekFormat;
        Q_EMIT firstDayOfWeekFormatChanged(firstDayOfWeekFormat);
    }
}

void DatetimeModel::setShortDateFormat(const QString &shortDateFormat)
{
    if (m_shortDateFormat != shortDateFormat) {
        m_shortDateFormat = shortDateFormat;
        Q_EMIT shortDateFormatChanged(shortDateFormat);
    }
}

void DatetimeModel::setLongDateFormat(const QString &longDateFormat)
{
    if (m_longDateFormat != longDateFormat) {
        m_longDateFormat = longDateFormat;
        Q_EMIT longDateFormatChanged(longDateFormat);
    }
}

void DatetimeModel::setShortTimeFormat(const QString &shortTimeFormat)
{
    if (m_shortTimeFormat != shortTimeFormat) {
        m_shortTimeFormat = shortTimeFormat;
        Q_EMIT shortTimeFormatChanged(shortTimeFormat);
    }
}

void DatetimeModel::setLongTimeFormat(const QString &longTimeFormat)
{
    if (m_longTimeFormat != longTimeFormat) {
        m_longTimeFormat = longTimeFormat;
        Q_EMIT longTimeFormatChanged(longTimeFormat);
    }
}

void DatetimeModel::setCurrencyFormat(const QString &currencyFormat)
{
    if (m_currencyFormat != currencyFormat) {
        m_currencyFormat = currencyFormat;
        Q_EMIT currencyFormatChanged(currencyFormat);
    }
}

void DatetimeModel::setNumberFormat(const QString &numberFormat)
{
    if (m_numberFormat != numberFormat) {
        m_numberFormat = numberFormat;
        Q_EMIT numberFormatChanged(numberFormat);
    }
}

void DatetimeModel::setPaperFormat(const QString &paperFormat)
{
    if (m_paperFormat != paperFormat) {
        m_paperFormat = paperFormat;
        Q_EMIT paperFormatChanged(paperFormat);
    }
}

void DatetimeModel::setRegionFormat(const RegionFormat &regionFormat)
{
    if (m_regionFormat != regionFormat) {
        m_regionFormat = regionFormat;
    }
}

void DatetimeModel::setCountries(const QStringList &countries)
{
    if (m_countries != countries) {
        m_countries = countries;
    }
}

void DatetimeModel::setRegions(const Regions &regions)
{
    if (m_regions != regions) {
        m_regions = regions;
    }
}
