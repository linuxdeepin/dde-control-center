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

    void activate();
    void deactivate();
    DatetimeModel *model() { return m_model; }

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
    void removeUserTimeZone(const ZoneInfo &info);
    void addUserTimeZone(const QString &zone);
#endif
    void setNtpServer(QString server);

    void setWeekdayFormat(int type);
    void setShortDateFormat(int type);
    void setLongDateFormat(int type);
    void setLongTimeFormat(int type);
    void setShortTimeFormat(int type);
    void setWeekStartDayFormat(int type);

    void genLocale(const QString &localeName);

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
    ZoneInfo GetZoneInfo(const QString &zoneId);
    void initRegionFormatData();
    std::optional<QStringList> getSupportedLocale();

private:
    DatetimeModel *m_model;
    DatetimeDBusProxy *m_timedateInter;
    QDateTime *m_setDatetime;
    RegionProxy *m_regionInter;
    DTK_CORE_NAMESPACE::DConfig *m_config;
    std::optional<QStringList> m_supportedLocaleList;
};

#endif // DATETIMEWORKER_H
