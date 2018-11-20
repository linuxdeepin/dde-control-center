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

#ifndef DATETIMEWORK_H
#define DATETIMEWORK_H

#include "datetimemodel.h"

#include <QObject>
#include <com_deepin_daemon_timedate.h>

namespace dcc {
namespace datetime {

using Timedate = com::deepin::daemon::Timedate;

class DatetimeWork : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWork(DatetimeModel* model, QObject *parent = 0);
    ~DatetimeWork();

    void activate();
    void deactivate();

Q_SIGNALS:
    void requestSetAutoHide(const bool visible) const;

public Q_SLOTS:
    void setNTP(bool ntp);
    void setDatetime(const QDateTime &time);
#ifndef DCC_DISABLE_TIMEZONE
    void setTimezone(const QString& timezone);
    void removeUserTimeZone(const ZoneInfo &info);
    void addUserTimeZone(const QString &zone);
#endif

private Q_SLOTS:
#ifndef DCC_DISABLE_TIMEZONE
    void onTimezoneListChanged(const QStringList &timezones);
#endif

private:
    DatetimeModel* m_model;
    Timedate* m_timedateInter;
};
}
}
#endif // DATETIMEWORK_H
