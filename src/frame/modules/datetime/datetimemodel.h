// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>

#include <types/zoneinfo.h>

namespace dcc {
namespace datetime {

class DatetimeModel : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeModel(QObject *parent = nullptr);

    inline bool nTP() const { return m_ntp; }
    void setNTP(bool ntp);
    inline bool get24HourFormat() const { return m_bUse24HourType; }
    void set24HourFormat(bool state);

    QList<ZoneInfo> userTimeZones() const;
    void addUserTimeZone(const ZoneInfo &zone);
    void removeUserTimeZone(const ZoneInfo &zone);
#ifndef DCC_DISABLE_TIMEZONE
    QString systemTimeZoneId() const;
    void setSystemTimeZoneId(const QString &systemTimeZoneId);
#endif
    inline ZoneInfo currentTimeZone() const { return m_currentTimeZone; }
    void setCurrentTimeZone(const ZoneInfo &currentTimeZone);

    inline ZoneInfo currentSystemTimeZone() const { return m_currentSystemTimeZone; }
    void setCurrentUseTimeZone(const ZoneInfo &currentTimeZone);

    inline QString ntpServerAddress() const { return  m_strNtpServerAddress; }
    void setNtpServerAddress(const QString &ntpServer);

    inline QStringList ntpServerList() const { return  m_NtpServerList; }
    void setNTPServerList(const QStringList &list);

    inline QString getTimeZone() const { return m_timeZones; }
    void setTimeZoneInfo(const QString &timeZone);

    inline int weekdayFormatType() const { return  m_weekdayFormatType; }
    inline int shortDateFormat() const { return  m_shortDateFormat; }
    inline int longDateFormat() const { return  m_longDateFormat; }
    inline int longTimeFormat() const { return  m_longTimeFormat; }
    inline int shorTimeFormat() const { return  m_shorTimeFormat; }
    inline int weekStartDayFormat() const { return  m_weekStartDayFormat; }
    inline int weekdayFormatTypeCount() const { return  m_weekdayFormatTypeCount; }
    inline int shortDateFormatTypeCount() const { return  m_shortDateFormatTypeCount; }
    inline int longDateFormatTypeCount() const { return  m_longDateFormatTypeCount; }
    inline int longTimeFormatTypeCount() const { return  m_longTimeFormatTypeCount; }
    inline int shorTimeFormatTypeCount() const { return  m_shorTimeFormatTypeCount; }
    inline int weekStartDayFormatTypeCount() const { return  m_weekStartDayFormatTypeCount; }

    inline QString currencySymbol() const { return  m_currencySymbol; }
    inline QString decimalSymbol() const { return  m_decimalSymbol; }
    inline QString digitGrouping() const { return  m_digitGrouping; }
    inline QString digitGroupingSymbol() const { return  m_digitGroupingSymbol; }
    inline QString negativeCurrencyFormat() const { return  m_negativeCurrencyFormat; }
    inline QString positiveCurrencyFormat() const { return  m_positiveCurrencyFormat; }

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
    void setFormatFormWidget(const QString &type, QString value, int place = 0);
    void setCurrencySymbol(const QString &value);
    void setDecimalSymbol(const QString &value);
    void setDigitGrouping(const QString &value);
    void setDigitGroupingSymbol(const QString &value);
    void setNegativeCurrencyFormat(const QString &value);
    void setPositiveCurrencyFormat(const QString &value);

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
    void formatPropertyChanged(QString property, QString value);//used widget set to work
    void CurrencySymbolChanged(const QString &value);
    void DecimalSymbolChanged(const QString &value);
    void DigitGroupingChanged(const QString &value);
    void DigitGroupingSymbolChanged(const QString &value);
    void NegativeCurrencyFormatChanged(const QString &value);
    void PositiveCurrencyFormatChanged(const QString &value);

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

    int m_weekdayFormatType{0};
    int m_shortDateFormat{0};
    int m_longDateFormat{0};
    int m_longTimeFormat{0};
    int m_shorTimeFormat{0};
    int m_weekStartDayFormat{0};

    int m_weekdayFormatTypeCount{0};
    int m_shortDateFormatTypeCount{0};
    int m_longDateFormatTypeCount{0};
    int m_longTimeFormatTypeCount{0};
    int m_shorTimeFormatTypeCount{0};
    int m_weekStartDayFormatTypeCount{0};

    QString m_currencySymbol{""};
    QString m_decimalSymbol{""};
    QString m_digitGrouping{""};
    QString m_digitGroupingSymbol{""};
    QString m_negativeCurrencyFormat{""};
    QString m_positiveCurrencyFormat{""};
};

}
}
#endif // DATETIMEMODEL_H
