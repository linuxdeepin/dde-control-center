//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DATETIMEWORKER_H
#define DATETIMEWORKER_H

#include "datetimemodel.h"
#include "datetimedbusproxy.h"

#include <QObject>

#include <optional>

#include <dtkcore_global.h>
#include <qvariant.h>

class DatetimeDBusProxy;
class RegionProxy;

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

class DatetimeWorker : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWorker(DatetimeModel *model, QObject *parent = nullptr);
    ~DatetimeWorker();

    Q_INVOKABLE void activate();
    void deactivate();
    DatetimeModel *model();

    std::optional<LocaleList> getAllLocale();
    std::optional<QString> getLocaleRegion();

    void setLocaleRegion(const QString &locale);
    void setConfigValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void requestSetAutoHide(const bool visible) const;

public Q_SLOTS:
    void setNTP(bool ntp);
    void setDatetime(const QDateTime &time);
    void set24HourType(bool state);
#ifndef DCC_DISABLE_TIMEZONE
    void setTimezone(const QString &timezone);
    void removeUserTimeZone(const QString &zone);
    void addUserTimeZone(const QString &zone);
#endif
    void setNtpServer(QString server);
    QString getCustomNtpServer();

    int weekdayFormat();
    void setWeekdayFormat(int type);
    void setShortDateFormat(int type);
    void setLongDateFormat(int type);
    void setLongTimeFormat(int type);
    void setShortTimeFormat(int type);
    void setWeekStartDayFormat(int type);

    bool genLocale(const QString &localeName);

    ZoneInfo GetZoneInfo(const QString &zoneId);


    QString decimalSymbol();
    void setDecimalSymbol(const QString &value);

    QString digitGrouping();
    void setDigitGrouping(const QString &value);

    QString digitGroupingSymbol();
    void setDigitGroupingSymbol(const QString &value);

    QString currencySymbol();
    void setCurrencySymbol(const QString &value);

    QString negativeCurrencyFormat();
    void setNegativeCurrencyFormat(const QString &value);

    QString positiveCurrencyFormat();
    void setPositiveCurrencyFormat(const QString &value);

private Q_SLOTS:
#ifndef DCC_DISABLE_TIMEZONE
    void onTimezoneListChanged(const QStringList &timezones);
#endif
    void setAutoHide();
    void setNTPError();
    void setDatetimeStart();
    void setDateFinished();
    void getSampleNTPServersFinished(const QStringList &serverList);
    void SetNTPServerFinished();
    void SetNTPServerError();
    void getZoneInfoFinished(ZoneInfo zoneInfo);

private:
    void refreshNtpServerList();
    void initRegionFormatData();
    std::optional<QStringList> getSupportedLocale();

private:
    DatetimeModel *m_model;
    DatetimeDBusProxy *m_timedateInter;
    QDateTime *m_setDatetime;
    RegionProxy *m_regionInter;
    DTK_CORE_NAMESPACE::DConfig *m_config;
    DTK_CORE_NAMESPACE::DConfig *m_datetimeConfig;
    DTK_CORE_NAMESPACE::DConfig *m_daemonTimedateConfig;
    std::optional<QStringList> m_supportedLocaleList;
};

#endif // DATETIMEWORKER_H
