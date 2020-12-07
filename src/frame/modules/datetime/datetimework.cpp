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

static ZoneInfo callbackZoneInfo(const QString &zoneId)
{
    return DatetimeWork::getInstance().getTimedate()->GetZoneInfo(zoneId);
}

DatetimeWork::DatetimeWork(DatetimeModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_timedateInter(new Timedate("com.deepin.daemon.Timedate", "/com/deepin/daemon/Timedate", QDBusConnection::sessionBus(), this))
{
    m_timedateInter->setSync(false);

#ifndef DCC_DISABLE_TIMEZONE
    connect(m_timedateInter, &__Timedate::UserTimezonesChanged, this, &DatetimeWork::onTimezoneListChanged);
    connect(m_timedateInter, &__Timedate::TimezoneChanged, m_model, &DatetimeModel::setSystemTimeZoneId);
#endif
    connect(m_timedateInter, &__Timedate::NTPChanged, m_model, &DatetimeModel::setNTP);
    connect(m_timedateInter, &__Timedate::Use24HourFormatChanged, m_model, &DatetimeModel::set24HourFormat);

    connect(m_timedateInter, &__Timedate::TimezoneChanged, this, [ = ]() {
        m_model->setCurrentUseTimeZone(GetZoneInfo(QTimeZone::systemTimeZoneId()));
    });

    connect(m_timedateInter, &__Timedate::NTPServerChanged, m_model, &DatetimeModel::setNtpServerAddress);
    connect(m_timedateInter, &__Timedate::TimezoneChanged, m_model, &DatetimeModel::setTimeZoneInfo);

    m_model->setCurrentTimeZone(GetZoneInfo(QTimeZone::systemTimeZoneId()));
    m_model->setCurrentUseTimeZone(GetZoneInfo(QTimeZone::systemTimeZoneId()));
    m_model->set24HourFormat(m_timedateInter->use24HourFormat());

    m_model->setWeekdayFormatType(m_timedateInter->weekdayFormat());
    m_model->setLongDateFormat(m_timedateInter->longDateFormat());
    m_model->setShortDateFormat(m_timedateInter->shortDateFormat());
    m_model->setShorTimeFormat(m_timedateInter->shortTimeFormat());
    m_model->setLongTimeFormat(m_timedateInter->longTimeFormat());
    m_model->setWeekStartDayFormat(m_timedateInter->weekBegins());

    m_model->setWeekdayFormatTypeCount(3);
    m_model->setLongDateFormatTypeCount(3);
    m_model->setShortDateFormatTypeCount(9);
    m_model->setShorTimeFormatTypeCount(4);
    m_model->setLongTimeFormatTypeCount(4);
    m_model->setWeekStartDayFormatTypeCount(2);

    connect(m_timedateInter, &Timedate::WeekdayFormatChanged, m_model, &DatetimeModel::setWeekdayFormatType);
    connect(m_timedateInter, &Timedate::LongDateFormatChanged, m_model, &DatetimeModel::setLongDateFormat);
    connect(m_timedateInter, &Timedate::ShortDateFormatChanged, m_model, &DatetimeModel::setShortDateFormat);
    connect(m_timedateInter, &Timedate::ShortTimeFormatChanged, m_model, &DatetimeModel::setShorTimeFormat);
    connect(m_timedateInter, &Timedate::LongTimeFormatChanged, m_model, &DatetimeModel::setLongTimeFormat);
    connect(m_timedateInter, &Timedate::WeekBeginsChanged, m_model, &DatetimeModel::setWeekStartDayFormat);
    refreshNtpServerList();
    m_model->setNtpServerAddress(m_timedateInter->nTPServer());
    m_model->setTimeZoneInfo(m_timedateInter->timezone());
}

DatetimeWork::~DatetimeWork()
{

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

Timedate *DatetimeWork::getTimedate()
{
    return m_timedateInter;
}

DatetimeWork &DatetimeWork::getInstance()
{
    static DatetimeWork worker(new DatetimeModel);
    return worker;
}

void DatetimeWork::setNTP(bool ntp)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_timedateInter->SetNTP(ntp);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
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
    qDebug() << "start setDatetime";
    QDBusPendingCall call = m_timedateInter->SetNTP(false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (!call.isError()) {
            const QDate date = datetime.date();
            const QTime time = datetime.time();
            qDebug() << "set ntp success, m_timedateInter->SetDate";
            QDBusPendingCall call1 = m_timedateInter->SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), 0, 0);
            QDBusPendingCallWatcher *watcher1 = new QDBusPendingCallWatcher(call1, this);
            connect(watcher1, &QDBusPendingCallWatcher::finished, this, [ = ] {
                if (!call1.isError()) {
                    Q_EMIT m_model->systemTimeChanged();
                } else {
                    qDebug() << "m_timedateInter->SetDate failed : " << call.error().message();
                }
                watcher1->deleteLater();
            });
        } else {
            qDebug() << "disable ntp failed : " << call.error().message();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void DatetimeWork::set24HourType(bool state)
{
    m_timedateInter->setUse24HourFormat(state);
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

void DatetimeWork::setNtpServer(QString server)
{
    qDebug() << "Try set server : " << server;

    if (server == m_timedateInter->nTPServer())
        return;

    QDBusPendingCall call = m_timedateInter->SetNTPServer(server);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        //call.isError() : true表示取消，false表示确定
        if (!call.isError()) {
            qDebug() << "set server success.";
            Q_EMIT m_model->NTPServerChanged(m_timedateInter->nTPServer());
        } else {
            qDebug() << "Not set server success.";
            Q_EMIT m_model->NTPServerNotChanged(m_timedateInter->nTPServer());
        }

        watcher->deleteLater();
    });
}

void DatetimeWork::setWeekdayFormat(int type)
{
    m_timedateInter->setWeekdayFormat(type);
}

void DatetimeWork::setShortDateFormat(int type)
{
    m_timedateInter->setShortDateFormat(type);
}

void DatetimeWork::setLongDateFormat(int type)
{
   m_timedateInter->setLongDateFormat(type);
}

void DatetimeWork::setLongTimeFormat(int type)
{
    m_timedateInter->setLongTimeFormat(type);
}

void DatetimeWork::setShortTimeFormat(int type)
{
    m_timedateInter->setShortTimeFormat(type);
}

void DatetimeWork::setWeekStartDayFormat(int type)
{
    m_timedateInter->setWeekBegins(type);
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

    QFuture<ZoneInfo> future = QtConcurrent::mapped(timezones, callbackZoneInfo);
    watcher->setFuture(future);
}
#endif

void DatetimeWork::refreshNtpServerList()
{
    QDBusPendingCall call = m_timedateInter->GetSampleNTPServers();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (call.isError()) {
            qDebug() << Q_FUNC_INFO << " Failed to get ntpserver list.";
        } else {
            QDBusReply<QStringList> reply = call.reply();
            m_model->setNTPServerList(reply.value());
        }
        watcher->deleteLater();
    });
}

ZoneInfo DatetimeWork::GetZoneInfo(const QString &zoneId)
{
    return m_timedateInter->GetZoneInfo(zoneId);
}

}
}
