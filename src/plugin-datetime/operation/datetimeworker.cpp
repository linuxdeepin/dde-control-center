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

#include "datetimeworker.h"
#include "datetimedbusproxy.h"
#include <QDebug>

#include <QtConcurrent>
#include <QFutureWatcher>

DatetimeWorker::DatetimeWorker(DatetimeModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_timedateInter(new DatetimeDBusProxy(this))
{
#ifndef DCC_DISABLE_TIMEZONE
    connect(m_timedateInter, &DatetimeDBusProxy::UserTimezonesChanged, this, &DatetimeWorker::onTimezoneListChanged);
    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, m_model, &DatetimeModel::setSystemTimeZoneId);
#endif
    connect(m_timedateInter, &DatetimeDBusProxy::NTPChanged, m_model, &DatetimeModel::setNTP);
    connect(m_timedateInter, &DatetimeDBusProxy::Use24HourFormatChanged, m_model, &DatetimeModel::set24HourFormat);

    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, this, [=](const QString &value) {
        auto tzinfo = GetZoneInfo(value);
        if (tzinfo.getZoneName().length() == 0) {
            tzinfo = GetZoneInfo(QTimeZone::systemTimeZoneId());
        }
        m_model->setCurrentUseTimeZone(tzinfo);
    });

    connect(m_timedateInter, &DatetimeDBusProxy::NTPServerChanged, m_model, &DatetimeModel::setNtpServerAddress);
    connect(m_timedateInter, &DatetimeDBusProxy::TimezoneChanged, m_model, &DatetimeModel::setTimeZoneInfo);

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

    connect(m_timedateInter, &DatetimeDBusProxy::WeekdayFormatChanged, m_model, &DatetimeModel::setWeekdayFormatType);
    connect(m_timedateInter, &DatetimeDBusProxy::LongDateFormatChanged, m_model, &DatetimeModel::setLongDateFormat);
    connect(m_timedateInter, &DatetimeDBusProxy::ShortDateFormatChanged, m_model, &DatetimeModel::setShortDateFormat);
    connect(m_timedateInter, &DatetimeDBusProxy::ShortTimeFormatChanged, m_model, &DatetimeModel::setShorTimeFormat);
    connect(m_timedateInter, &DatetimeDBusProxy::LongTimeFormatChanged, m_model, &DatetimeModel::setLongTimeFormat);
    connect(m_timedateInter, &DatetimeDBusProxy::WeekBeginsChanged, m_model, &DatetimeModel::setWeekStartDayFormat);
    refreshNtpServerList();
    m_model->setNtpServerAddress(m_timedateInter->nTPServer());
    m_model->setTimeZoneInfo(m_timedateInter->timezone());
    m_model->setNTP(m_timedateInter->nTP());
}

DatetimeWorker::~DatetimeWorker()
{
}

void DatetimeWorker::activate()
{
    m_model->setNTP(m_timedateInter->nTP());
#ifndef DCC_DISABLE_TIMEZONE
    m_model->setSystemTimeZoneId(m_timedateInter->timezone());
    onTimezoneListChanged(m_timedateInter->userTimezones());
#endif
}

void DatetimeWorker::deactivate()
{
}

void DatetimeWorker::setNTP(bool ntp)
{
    Q_EMIT requestSetAutoHide(false);
    m_timedateInter->SetNTP(ntp, this, SLOT(setAutoHide()), SLOT(setNTPError()));
}
void DatetimeWorker::setAutoHide()
{
    Q_EMIT requestSetAutoHide(true);
}
void DatetimeWorker::setNTPError()
{
    Q_EMIT m_model->NTPChanged(m_model->nTP());
    setAutoHide();
}

void DatetimeWorker::setDatetime(const QDateTime &datetime)
{
    Q_EMIT requestSetAutoHide(false);
    qDebug() << "start setDatetime";
    m_setDatetime = new QDateTime(datetime);
    m_timedateInter->SetNTP(false, this, SLOT(setDatetimeStart()), SLOT(setAutoHide()));
}
void DatetimeWorker::setDatetimeStart()
{
    if (m_setDatetime) {
        qDebug() << "set ntp success, m_timedateInter->SetDate";
        m_timedateInter->SetDate(*m_setDatetime, this, SLOT(setDateFinished()));
        delete m_setDatetime;
        m_setDatetime = nullptr;
    }
    setAutoHide();
}
void DatetimeWorker::setDateFinished()
{
    Q_EMIT m_model->systemTimeChanged();
}
void DatetimeWorker::set24HourType(bool state)
{
    m_timedateInter->setUse24HourFormat(state);
}

#ifndef DCC_DISABLE_TIMEZONE
void DatetimeWorker::setTimezone(const QString &timezone)
{
    m_timedateInter->SetTimezone(timezone, tr("Authentication is required to set the system timezone"));
}

void DatetimeWorker::removeUserTimeZone(const ZoneInfo &info)
{
    m_timedateInter->DeleteUserTimezone(info.getZoneName());
}

void DatetimeWorker::addUserTimeZone(const QString &zone)
{
    m_timedateInter->AddUserTimezone(zone);
}

void DatetimeWorker::setNtpServer(QString server)
{
    qInfo() << "Try set server : " << server;

    if (server.isEmpty() && server == m_timedateInter->nTPServer())
        return;
    m_timedateInter->SetNTPServer(server, tr("Authentication is required to change NTP server"), this, SLOT(SetNTPServerFinished()), SLOT(SetNTPServerError()));
}

void DatetimeWorker::SetNTPServerFinished()
{
    qInfo() << "set server success.";
    Q_EMIT m_model->NTPServerChanged(m_timedateInter->nTPServer());
}
void DatetimeWorker::SetNTPServerError()
{
    qInfo() << "Not set server success.";
    Q_EMIT m_model->NTPServerNotChanged(m_timedateInter->nTPServer());
}

void DatetimeWorker::setWeekdayFormat(int type)
{
    m_timedateInter->setWeekdayFormat(type);
}

void DatetimeWorker::setShortDateFormat(int type)
{
    m_timedateInter->setShortDateFormat(type);
}

void DatetimeWorker::setLongDateFormat(int type)
{
    m_timedateInter->setLongDateFormat(type);
}

void DatetimeWorker::setLongTimeFormat(int type)
{
    m_timedateInter->setLongTimeFormat(type);
}

void DatetimeWorker::setShortTimeFormat(int type)
{
    m_timedateInter->setShortTimeFormat(type);
}

void DatetimeWorker::setWeekStartDayFormat(int type)
{
    m_timedateInter->setWeekBegins(type);
}

void DatetimeWorker::onTimezoneListChanged(const QStringList &timezones)
{
    QList<ZoneInfo> zones = m_model->userTimeZones();
    QStringList zonesName;
    for (const ZoneInfo &zone : zones) {
        zonesName.append(zone.getZoneName());
    }
    for (const QString &zone : timezones) {
        zonesName.removeOne(zone);
        m_timedateInter->GetZoneInfo(zone, this, SLOT(getZoneInfoFinished(ZoneInfo)));
    }
    for (const ZoneInfo &zone : zones) {
        if (zonesName.contains(zone.getZoneName())) {
            m_model->removeUserTimeZone(zone);
        }
    }
}

void DatetimeWorker::getZoneInfoFinished(ZoneInfo zoneInfo)
{
    m_model->addUserTimeZone(zoneInfo);
}
#endif

void DatetimeWorker::refreshNtpServerList()
{

    m_timedateInter->GetSampleNTPServers(this, SLOT(getSampleNTPServersFinished(const QStringList &)));
}

void DatetimeWorker::getSampleNTPServersFinished(const QStringList &serverList)
{
    m_model->setNTPServerList(serverList);
}

ZoneInfo DatetimeWorker::GetZoneInfo(const QString &zoneId)
{
    return m_timedateInter->GetZoneInfo(zoneId);
}
