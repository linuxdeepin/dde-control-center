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
#ifndef DATETIME_DBUS_H
#define DATETIME_DBUS_H

#include "types/zoneinfo.h"

#include <QDBusContext>
#include <QObject>
#include <QDBusInterface>
#include <QJsonDocument>

#define DATETIME_SERVICE_NAME "com.deepin.daemon.Timedate"
#define DATETIME_SERVICE_PATH "/com/deepin/daemon/Timedate"

//struct DSTInfo {
//    // The timestamp of entering DST every year
//    qint64 Enter;

//    // The timestamp of leaving DST every year
//    qint64 Leave;

//    // The DST offset
//    int Offset;
//};

//struct Zone {
//    // Timezone name, ex: "Asia/Shanghai"
//    QString Name;

//    // Timezone description, ex: "上海"
//    QString Desc;

//    // Timezone offset
//    int Offset;

//    DSTInfo DST;
//};

class TimeDate : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DATETIME_SERVICE_NAME)

public:
    TimeDate(QObject *parent = nullptr);
    virtual ~TimeDate();

    Q_PROPERTY(bool CanNTP READ canNTP NOTIFY CanNTPChanged)
    inline bool canNTP() { return m_canNTP; }

    Q_PROPERTY(int DSTOffset READ dSTOffset WRITE setDSTOffset NOTIFY DSTOffsetChanged)
    inline int dSTOffset() { return m_DSTOffset; };
    inline void setDSTOffset(int value) { m_DSTOffset = value; }

    Q_PROPERTY(bool LocalRTC READ localRTC NOTIFY LocalRTCChanged)
    inline bool localRTC() { return m_localRTC; }

    Q_PROPERTY(int LongDateFormat READ longDateFormat WRITE setLongDateFormat NOTIFY LongDateFormatChanged)
    inline int longDateFormat() { return m_longDateFormat; }
    inline void setLongDateFormat(int value) { m_longDateFormat = value; }

    Q_PROPERTY(int LongTimeFormat READ longTimeFormat WRITE setLongTimeFormat NOTIFY LongTimeFormatChanged)
    inline int longTimeFormat() { return m_longTimeFormat; }
    inline void setLongTimeFormat(int value) { m_longTimeFormat = value; }

    Q_PROPERTY(bool NTP READ nTP NOTIFY NTPChanged)
    inline bool nTP() { return m_NTP; }

    Q_PROPERTY(QString NTPServer READ nTPServer NOTIFY NTPServerChanged)
    inline QString nTPServer() { return m_NTPServer; }

    Q_PROPERTY(int ShortDateFormat READ shortDateFormat WRITE setShortDateFormat NOTIFY ShortDateFormatChanged)
    inline int shortDateFormat() { return m_shortDateFormat; }
    inline void setShortDateFormat(int value) { m_shortDateFormat = value; }

    Q_PROPERTY(int ShortTimeFormat READ shortTimeFormat WRITE setShortTimeFormat NOTIFY ShortTimeFormatChanged)
    inline int shortTimeFormat() { return m_shortTimeFormat; }
    inline void setShortTimeFormat(int value) { m_shortTimeFormat = value; }

    Q_PROPERTY(QString Timezone READ timezone NOTIFY TimezoneChanged)
    inline QString timezone() { return m_timeZone; }

    Q_PROPERTY(bool Use24HourFormat READ use24HourFormat WRITE setUse24HourFormat NOTIFY Use24HourFormatChanged)
    inline bool use24HourFormat() { return m_use24HourFormat; }
    inline void setUse24HourFormat(bool value) { m_use24HourFormat = value; }

    Q_PROPERTY(QStringList UserTimezones READ userTimezones NOTIFY UserTimezonesChanged)
    inline QStringList userTimezones() { return m_userTimeZones; }

    Q_PROPERTY(int WeekBegins READ weekBegins WRITE setWeekBegins NOTIFY WeekBeginsChanged)
    inline int weekBegins() { return m_weekBegins; }
    inline void setWeekBegins(int value) { m_weekBegins = value; }

    Q_PROPERTY(int WeekdayFormat READ weekdayFormat WRITE setWeekdayFormat NOTIFY WeekdayFormatChanged)
    inline int weekdayFormat() { return m_weekdayFormat; }
    inline void setWeekdayFormat(int value) { m_weekdayFormat = value; }

public Q_SLOTS:
    void AddUserTimezone(QString);
    void DeleteUserTimezone(QString);
    ZoneInfo GetZoneInfo(QString);
    QStringList GetZoneList();
    QStringList GetSampleNTPServers();
    void SetDate(int, int, int, int, int, int, int);
    void SetLocalRTC(bool, bool);
    void SetNTP(bool);
    void SetNTPServer(const QString &);
    void SetTime(qlonglong, bool);
    void SetTimezone(const QString &);

Q_SIGNALS:
    // begin property changed signals
    void CanNTPChanged(bool value) const;
    void DSTOffsetChanged(int value) const;
    void LocalRTCChanged(bool value) const;
    void LongDateFormatChanged(int value) const;
    void LongTimeFormatChanged(int value) const;
    void NTPChanged(bool value) const;
    void NTPServerChanged(const QString &value) const;
    void ShortDateFormatChanged(int value) const;
    void ShortTimeFormatChanged(int value) const;
    void TimezoneChanged(const QString &value) const;
    void Use24HourFormatChanged(bool value) const;
    void UserTimezonesChanged(const QStringList &value) const;
    void WeekBeginsChanged(int value) const;
    void WeekdayFormatChanged(int value) const;

private:
    bool m_canNTP {false};
    bool m_NTP {false};
    bool m_localRTC {false};
    QString m_timeZone {QString()};
    bool m_use24HourFormat {false};
    int m_DSTOffset {0};
    QStringList m_userTimeZones {QStringList()};
    QString m_NTPServer {QString()};
    int m_weekdayFormat {0};
    int m_shortDateFormat {0};
    int m_longDateFormat {0};
    int m_shortTimeFormat {0};
    int m_longTimeFormat {0};
    int m_weekBegins {0};
};

#endif
