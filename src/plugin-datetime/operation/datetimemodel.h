//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>

#include "zoneinfo.h"

class DatetimeModel : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeModel(QObject *parent = nullptr);

    inline bool nTP() const { return m_ntp; }
    void setNTP(bool ntp);
    inline bool get24HourFormat() const { return m_bUse24HourType; }

    QList<ZoneInfo> userTimeZones() const;
    void addUserTimeZone(const ZoneInfo &zone);
    void removeUserTimeZone(const ZoneInfo &zone);
#ifndef DCC_DISABLE_TIMEZONE
    QString systemTimeZoneId() const;
    void setSystemTimeZoneId(const QString &systemTimeZoneId);
#endif
    inline ZoneInfo currentTimeZone() const
    {
        return m_currentTimeZone;
    }
    void setCurrentTimeZone(const ZoneInfo &currentTimeZone);

    inline ZoneInfo currentSystemTimeZone() const { return m_currentSystemTimeZone; }
    void setCurrentUseTimeZone(const ZoneInfo &currentTimeZone);

    inline QString ntpServerAddress() const { return m_strNtpServerAddress; }
    void setNtpServerAddress(const QString &ntpServer);

    inline QStringList ntpServerList() const { return m_NtpServerList; }
    void setNTPServerList(const QStringList &list);

    inline QString getTimeZone() const { return m_timeZones; }
    void setTimeZoneInfo(const QString &timeZone);

    inline int weekdayFormatType() const { return m_weekdayFormatType; }
    inline int shortDateFormat() const { return m_shortDateFormat; }
    inline int longDateFormat() const { return m_longDateFormat; }
    inline int longTimeFormat() const { return m_longTimeFormat; }
    inline int shorTimeFormat() const { return m_shorTimeFormat; }
    inline int weekStartDayFormat() const { return m_weekStartDayFormat; }
    inline int weekdayFormatTypeCount() const { return m_weekdayFormatTypeCount; }
    inline int shortDateFormatTypeCount() const { return m_shortDateFormatTypeCount; }
    inline int longDateFormatTypeCount() const { return m_longDateFormatTypeCount; }
    inline int longTimeFormatTypeCount() const { return m_longTimeFormatTypeCount; }
    inline int shorTimeFormatTypeCount() const { return m_shorTimeFormatTypeCount; }
    inline int weekStartDayFormatTypeCount() const { return m_weekStartDayFormatTypeCount; }

    void setWeekdayFormatType(int type);
    void setShortDateFormat(int type);
    void setLongDateFormat(int type);
    void setLongTimeFormat(int type);
    void setShorTimeFormat(int type);
    void setWeekStartDayFormat(int type);
    void setWeekdayFormatTypeCount(int size);
    void setShortDateFormatTypeCount(int size);
    void setLongDateFormatTypeCount(int size);
    void setLongTimeFormatTypeCount(int size);
    void setShorTimeFormatTypeCount(int size);
    void setWeekStartDayFormatTypeCount(int size);

Q_SIGNALS:
    void NTPChanged(bool value);
    void hourTypeChanged(bool value);
    void userTimeZoneAdded(const ZoneInfo &zone);
    void userTimeZoneRemoved(const ZoneInfo &zone);
    void systemTimeZoneIdChanged(const QString &zone);
    void systemTimeChanged();
    void currentTimeZoneChanged(const ZoneInfo &zone) const;
    void currentSystemTimeZoneChanged(const ZoneInfo &zone) const;
    void NTPServerChanged(QString server);
    void NTPServerListChanged(QStringList list);
    void NTPServerNotChanged(QString server);
    void weekdayFormatTypeChanged(int type);
    void shortDateFormatChanged(int type);
    void longDateFormatChanged(int type);
    void longTimeFormatChanged(int type);
    void shorTimeFormatChanged(int type);
    void weekStartDayFormatChanged(int type);
    void timeZoneChanged(QString value);

public Q_SLOTS:
    void set24HourFormat(bool state);

private:
    bool m_ntp;
    bool m_bUse24HourType;
    QStringList m_userZoneIds;
#ifndef DCC_DISABLE_TIMEZONE
    QString m_systemTimeZoneId;
#endif
    QList<ZoneInfo> m_userTimeZones;
    ZoneInfo m_currentTimeZone;
    ZoneInfo m_currentSystemTimeZone;
    QString m_strNtpServerAddress;
    QStringList m_NtpServerList;
    QString m_timeZones;

    int m_weekdayFormatType{ 0 };
    int m_shortDateFormat{ 0 };
    int m_longDateFormat{ 0 };
    int m_longTimeFormat{ 0 };
    int m_shorTimeFormat{ 0 };
    int m_weekStartDayFormat{ 0 };

    int m_weekdayFormatTypeCount{ 0 };
    int m_shortDateFormatTypeCount{ 0 };
    int m_longDateFormatTypeCount{ 0 };
    int m_longTimeFormatTypeCount{ 0 };
    int m_shorTimeFormatTypeCount{ 0 };
    int m_weekStartDayFormatTypeCount{ 0 };
};

#endif // DATETIMEMODEL_H
