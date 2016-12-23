#include "datetimework.h"
#include <QDebug>

#include <QtConcurrent>
#include <QFutureWatcher>

namespace dcc {
namespace datetime {

static Timedate *timedateInter = new Timedate("com.deepin.daemon.Timedate",
                                              "/com/deepin/daemon/Timedate",
                                              QDBusConnection::sessionBus());
static ZoneInfo GetZoneInfo (const QString &zoneId) {
    return timedateInter->GetZoneInfo(zoneId);
}

DatetimeWork::DatetimeWork(DatetimeModel *model, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_timedateInter(timedateInter)
{
    m_timedateInter->setSync(false);

    connect(m_timedateInter, &__Timedate::UserTimezonesChanged, this, &DatetimeWork::onTimezoneListChanged);
    connect(m_timedateInter, &__Timedate::TimezoneChanged, m_model, &DatetimeModel::setSystemTimeZoneId);
    connect(m_timedateInter, &__Timedate::NTPChanged, m_model, &DatetimeModel::setNTP);
}

void DatetimeWork::activate()
{
    m_model->setNTP(m_timedateInter->nTP());
    onTimezoneListChanged(m_timedateInter->userTimezones());
    m_model->setSystemTimeZoneId(m_timedateInter->timezone());
}

void DatetimeWork::deactivate()
{

}

void DatetimeWork::setTimezone(const QString &timezone)
{
    m_timedateInter->SetTimezone(timezone);
}

void DatetimeWork::setDatetime(const QDateTime &datetime)
{
    m_timedateInter->SetNTP(false);

    const QDate date = datetime.date();
    const QTime time = datetime.time();

    m_timedateInter->SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), 0, 0);
}

void DatetimeWork::setNTP(bool ntp)
{
    m_timedateInter->SetNTP(ntp);
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

}
}
