//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>

#include "zoneinfo.h"
#include "regionproxy.h"

class DatetimeModel : public QObject
{
    Q_OBJECT
public:
    using Regions = QMap<QString, QLocale>;

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

    inline QString country() const { return m_country; }
    void setCountry(const QString &country);

    inline QString localeName() const { return m_localeName; }
    void setLocaleName(const QString &localeName);

    inline QString langRegion() const { return m_langCountry; }
    void setLangRegion(const QString &langCountry);

    inline int firstDayOfWeekFormat() const { return m_firstDayOfWeekFormat; }
    void setFirstDayOfWeek(const int &firstDayOfWeekFormat);

    inline QString shortDateFormat() const { return m_shortDateFormat; }
    void setShortDateFormat(const QString &shortDateFormat);

    inline QString longDateFormat() const { return m_longDateFormat; }
    void setLongDateFormat(const QString &longDateFormat);

    inline QString shortTimeFormat() const { return m_shortTimeFormat; }
    void setShortTimeFormat(const QString &shortTimeFormat);

    inline QString longTimeFormat() const { return m_longTimeFormat; }
    void setLongTimeFormat(const QString &longTimeFormat);

    inline QString currencyFormat() const { return m_currencyFormat; }
    void setCurrencyFormat(const QString &currencyFormat);

    inline QString numberFormat() const { return m_numberFormat; }
    void setNumberFormat(const QString &numberFormat);

    inline QString paperFormat() const { return m_paperFormat; }
    void setPaperFormat(const QString &paperFormat);

    inline RegionFormat regionFormat() const { return m_regionFormat; }
    void setRegionFormat(const RegionFormat &regionFormat);

    inline QStringList countries() const { return m_countries; }
    void setCountries(const QStringList &countries);

    inline Regions regions() const { return m_regions; }
    void setRegions(const Regions &regions);

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
    void timeZoneChanged(QString value);
    void localeNameChanged(const QString &localeName);
    void countryChanged(const QString &country);
    void langCountryChanged(const QString &langCountry);
    void firstDayOfWeekFormatChanged(const int firstDayOfWeekFormat);
    void shortDateFormatChanged(const QString &shortDateFormat);
    void longDateFormatChanged(const QString &longDate);
    void shortTimeFormatChanged(const QString &shortTimeFormat);
    void longTimeFormatChanged(const QString &longTimeFormat);
    void currencyFormatChanged(const QString &currencyFormat);
    void numberFormatChanged(const QString &numberFormat);
    void paperFormatChanged(const QString &numberFormat);

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
    QString m_country;
    QString m_langCountry;
    QStringList m_countries;
    Regions m_regions;
    QString m_localeName;
    int m_firstDayOfWeekFormat;
    QString m_shortDateFormat;
    QString m_longDateFormat;
    QString m_shortTimeFormat;
    QString m_longTimeFormat;
    QString m_currencyFormat;
    QString m_numberFormat;
    QString m_paperFormat;
    RegionFormat m_regionFormat;
};

#endif // DATETIMEMODEL_H
