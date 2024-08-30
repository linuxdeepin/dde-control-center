// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INSTALLER_SYSINFO_TIMEZONE_H
#define INSTALLER_SYSINFO_TIMEZONE_H

#include <QHash>
#include <QList>

namespace installer {

struct ZoneInfo
{
public:
    QString country;
    QString timezone;

    // Coordinates of zone.
    double latitude;
    double longitude;

    // Distance to clicked point for comparison.
    double distance;
};

[[maybe_unused]] bool ZoneInfoDistanceComp(const ZoneInfo &a, const ZoneInfo &b);
QDebug &operator<<(QDebug &debug, const ZoneInfo &info);
typedef QList<ZoneInfo> ZoneInfoList;

// Read available timezone info in zone.tab file.
ZoneInfoList GetZoneInfoList();

// Find ZoneInfo based on |country| or |timezone|.
// Returns -1 if not found.
[[maybe_unused]] int GetZoneInfoByCountry(const ZoneInfoList &list, const QString &country);
int GetZoneInfoByZone(const ZoneInfoList &list, const QString &timezone);

// Read current timezone in /etc/timezone file.
QString GetCurrentTimezone();

// Returns name of timezone, excluding continent name.
[[maybe_unused]] QString GetTimezoneName(const QString &timezone);

// Returns local name of timezone, excluding continent name.
// |locale| is desired locale name.
QString GetLocalTimezoneName(const QString &timezone, const QString &locale);

// A map between old name of timezone and current name.
// e.g. Asia/Chongqing -> Asia/Shanghai
typedef QHash<QString, QString> TimezoneAliasMap;
[[maybe_unused]] TimezoneAliasMap GetTimezoneAliasMap();

// Validate |timezone|.
[[maybe_unused]] bool IsValidTimezone(const QString &timezone);

struct TimezoneOffset
{
    QString name; // Offset name, like CST.
    long seconds; // Offset seconds.
};

// Get |timezone| offset.
[[maybe_unused]] TimezoneOffset GetTimezoneOffset(const QString &timezone);

} // namespace installer

#endif // INSTALLER_SYSINFO_TIMEZONE_H
