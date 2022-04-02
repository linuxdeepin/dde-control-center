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

#ifndef DATETIMEWORKER_H
#define DATETIMEWORKER_H

#include "datetimemodel.h"

#include <QObject>

class DatetimeDBusProxy;

class DatetimeWorker : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWorker(DatetimeModel *model, QObject *parent = nullptr);
    ~DatetimeWorker();

    void activate();
    void deactivate();
    DatetimeModel *model() { return m_model; }

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

private:
    DatetimeModel *m_model;
    DatetimeDBusProxy *m_timedateInter;
    QDateTime *m_setDatetime;
};

#endif // DATETIMEWORKER_H
