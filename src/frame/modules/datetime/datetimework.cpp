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

#include "datetimework.h"
#include <QDebug>

#include <QtConcurrent>
#include <QFutureWatcher>

namespace dcc {
namespace datetime {

static Timedate *TimedateInter = nullptr;
static Timedate *timedateInter(QObject *parent = nullptr)
{
    static QMutex mutex;
    mutex.lock();
    if (!TimedateInter) {
        TimedateInter = new Timedate("com.deepin.daemon.Timedate",
                                     "/com/deepin/daemon/Timedate",
                                     QDBusConnection::sessionBus(),
                                     parent);
    }
    mutex.unlock();

    return TimedateInter;
}

static ZoneInfo GetZoneInfo (const QString &zoneId)
{
    return timedateInter()->GetZoneInfo(zoneId);
}

DatetimeWork::DatetimeWork(DatetimeModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_timedateInter(timedateInter(this))
{
    m_timedateInter->setSync(false);

#ifndef DCC_DISABLE_TIMEZONE
    connect(m_timedateInter, &__Timedate::UserTimezonesChanged, this, &DatetimeWork::onTimezoneListChanged);
    connect(m_timedateInter, &__Timedate::TimezoneChanged, m_model, &DatetimeModel::setSystemTimeZoneId);
#endif
    connect(m_timedateInter, &__Timedate::NTPChanged, m_model, &DatetimeModel::setNTP);

    m_model->setCurrentTimeZone(GetZoneInfo(QTimeZone::systemTimeZoneId()));
}

DatetimeWork::~DatetimeWork()
{
    if (TimedateInter)
        TimedateInter->deleteLater();
    TimedateInter = nullptr;
}

void DatetimeWork::activate()
{
    m_model->setNTP(m_timedateInter->nTP());
#ifndef DCC_DISABLE_TIMEZONE
    m_model->setSystemTimeZoneId(m_timedateInter->timezone());
    onTimezoneListChanged(m_timedateInter->userTimezones());
#endif

}

void DatetimeWork::deactivate()
{

}

void DatetimeWork::setNTP(bool ntp)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_timedateInter->SetNTP(ntp);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        // If the call failed, revert the UI change.
        if (call.isError()) {
            Q_EMIT m_model->NTPChanged(m_model->nTP());
        }
        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void DatetimeWork::setDatetime(const QDateTime &datetime)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_timedateInter->SetNTP(false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (!call.isError()) {
            const QDate date = datetime.date();
            const QTime time = datetime.time();

            QDBusPendingCall call1 = m_timedateInter->SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), 0, 0);
            QDBusPendingCallWatcher *watcher1 = new QDBusPendingCallWatcher(call1, this);
            connect(watcher1, &QDBusPendingCallWatcher::finished, this, [=] {
                if (!call1.isError()) {
                    Q_EMIT m_model->systemTimeChanged();
                }
                watcher1->deleteLater();
            });
        } else {
            qWarning() << "disable ntp failed : " << call.error().message();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

#ifndef DCC_DISABLE_TIMEZONE
void DatetimeWork::setTimezone(const QString &timezone)
{
    m_timedateInter->SetTimezone(timezone);
}

void DatetimeWork::removeUserTimeZone(const ZoneInfo &info)
{
    m_timedateInter->DeleteUserTimezone(info.getZoneName());
}

void DatetimeWork::addUserTimeZone(const QString &zone)
{
    m_timedateInter->AddUserTimezone(zone);
}

void DatetimeWork::onTimezoneListChanged(const QStringList &timezones)
{
    QFutureWatcher<ZoneInfo> *watcher = new QFutureWatcher<ZoneInfo>;
    connect(watcher, &QFutureWatcher<ZoneInfo>::finished, [this, watcher] {
        QFuture<ZoneInfo> future = watcher->future();
        QStringList records;

        for (int i = 0; i < future.resultCount(); i++) {
            ZoneInfo info = watcher->resultAt(i);
            m_model->addUserTimeZone(info);
            records.append(info.getZoneName());
        }

        for (const ZoneInfo &zone : m_model->userTimeZones()) {
            if (!records.contains(zone.getZoneName())) {
                m_model->removeUserTimeZone(zone);
            }
        }

        watcher->deleteLater();
    });

    QFuture<ZoneInfo> future = QtConcurrent::mapped(timezones, GetZoneInfo);
    watcher->setFuture(future);
}
#endif

}
}
