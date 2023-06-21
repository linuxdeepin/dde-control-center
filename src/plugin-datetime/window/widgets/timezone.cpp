// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "timezone.h"

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE /* For tm_gmtoff and tm_zone */
#endif
#include "consts.h"
#include "file_util.h"

#include <libintl.h>

#include <QDebug>

#include <cmath>

#include <locale.h>
#include <time.h>

namespace installer {

namespace {

const QString tzDirPath = std::visit([] {
    QString tzDirPath = "/usr/share/zoneinfo";
    if (qEnvironmentVariableIsSet("TZDIR"))
        tzDirPath = qEnvironmentVariable("TZDIR");
    return tzDirPath;
});

// Absolute path to zone.tab file.
const QString kZoneTabFile = std::visit([] {
    return tzDirPath + "/zone1970.tab";
});

// Absolute path to backward timezone file.
const char kTimezoneAliasFile[] = "/timezone_alias";

// Domain name for timezones.
const char kTimezoneDomain[] = "deepin-installer-timezones";

// Parse latitude and longitude of the zone's principal location.
// See https://en.wikipedia.org/wiki/List_of_tz_database_time_zones.
// |pos| is in ISO 6709 sign-degrees-minutes-seconds format,
// either +-DDMM+-DDDMM or +-DDMMSS+-DDDMMSS.
// |digits| 2 for latitude, 3 for longitude.
double ConvertPos(const QString &pos, int digits)
{
    if (pos.length() < 4 || digits > 9) {
        return 0.0;
    }

    const QString integer = pos.left(digits + 1);
    const QString fraction = pos.mid(digits + 1);
    const double t1 = integer.toDouble();
    const double t2 = fraction.toDouble();
    if (t1 > 0.0) {
        return t1 + t2 / pow(10.0, fraction.length());
    } else {
        return t1 - t2 / pow(10.0, fraction.length());
    }
}

} // namespace

[[maybe_unused]] bool ZoneInfoDistanceComp(const ZoneInfo &a, const ZoneInfo &b)
{
    return a.distance < b.distance;
}

QDebug &operator<<(QDebug &debug, const ZoneInfo &info)
{
    debug << "ZoneInfo {"
          << "cc:" << info.country << "tz:" << info.timezone << "lat:" << info.latitude
          << "lng:" << info.longitude << "}";
    return debug;
}

ZoneInfoList GetZoneInfoList()
{
    ZoneInfoList list;
    const QString content(ReadFile(kZoneTabFile));
    for (const QString &line : content.split('\n')) {
        if (!line.startsWith('#')) {
            const QStringList parts(line.split('\t'));
            // Parse latitude and longitude.
            if (parts.length() >= 3) {
                const QString coordinates = parts.at(1);
                int index = coordinates.indexOf('+', 3);
                if (index == -1) {
                    index = coordinates.indexOf('-', 3);
                }
                Q_ASSERT(index > -1);
                const double latitude = ConvertPos(coordinates.left(index), 2);
                const double longitude = ConvertPos(coordinates.mid(index), 3);
                const ZoneInfo zone_info = { parts.at(0), parts.at(2), latitude, longitude, 0.0 };
                list.append(zone_info);
            }
        }
    }
    return list;
}

[[maybe_unused]] int GetZoneInfoByCountry(const ZoneInfoList &list, const QString &country)
{
    int index = -1;
    for (const ZoneInfo &info : list) {
        index++;
        if (info.country == country) {
            return index;
        }
    }
    return -1;
}

int GetZoneInfoByZone(const ZoneInfoList &list, const QString &timezone)
{
    int index = -1;
    for (const ZoneInfo &info : list) {
        index++;
        if (info.timezone == timezone) {
            return index;
        }
    }
    return -1;
}

QString GetCurrentTimezone()
{
    const QString content(ReadFile("/etc/timezone"));
    return content.trimmed();
}

[[maybe_unused]] QString GetTimezoneName(const QString &timezone)
{
    const int index = timezone.lastIndexOf('/');
    return (index > -1) ? timezone.mid(index + 1) : timezone;
}

QString GetLocalTimezoneName(const QString &timezone, const QString &locale)
{
    // Set locale first.
    (void)setlocale(LC_ALL, QString(locale + ".UTF-8").toStdString().c_str());
    const QString local_name(dgettext(kTimezoneDomain, timezone.toStdString().c_str()));
    int index = local_name.lastIndexOf('/');
    if (index == -1) {
        // Some translations of locale name contains non-standard char.
        index = local_name.lastIndexOf("∕");
    }

    // Reset locale.
    (void)setlocale(LC_ALL, kDefaultLocale);

    return (index > -1) ? local_name.mid(index + 1) : local_name;
}

[[maybe_unused]] TimezoneAliasMap GetTimezoneAliasMap()
{
    TimezoneAliasMap map;

    const QString content = ReadFile(kTimezoneAliasFile);
    for (const QString &line : content.split('\n')) {
        if (!line.isEmpty()) {
            const QStringList parts = line.split(':');
            Q_ASSERT(parts.length() == 2);
            if (parts.length() == 2) {
                map.insert(parts.at(0), parts.at(1));
            }
        }
    }

    return map;
}

[[maybe_unused]] bool IsValidTimezone(const QString &timezone)
{
    // Ignores empty timezone.
    if (timezone.isEmpty()) {
        return false;
    }

    // If |filepath| is a file or a symbolic link to file, it is a valid timezone.
    const QString filepath(tzDirPath + QDir::separator() + timezone);
    return QFile::exists(filepath);
}

[[maybe_unused]] TimezoneOffset GetTimezoneOffset(const QString &timezone)
{
    const char *kTzEnv = "TZ";
    const char *old_tz = getenv(kTzEnv);
    setenv(kTzEnv, timezone.toStdString().c_str(), 1);
    struct tm tm;
    const time_t curr_time = time(nullptr);

    // Call tzset() before localtime_r(). Set tzset(3).
    tzset();
    (void)localtime_r(&curr_time, &tm);

    // Reset timezone.
    if (old_tz) {
        setenv(kTzEnv, old_tz, 1);
    } else {
        unsetenv(kTzEnv);
    }

    const TimezoneOffset offset = { tm.tm_zone, tm.tm_gmtoff };
    return offset;
}

} // namespace installer
