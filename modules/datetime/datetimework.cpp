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

#ifndef DCC_DISABLE_TIMEZONE
static ZoneInfo GetZoneInfo (const QString &zoneId)
{
    return timedateInter()->GetZoneInfo(zoneId);
}
#endif

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
    QDBusPendingCall call = m_timedateInter->SetNTP(ntp);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        // If the call failed, revert the UI change.
        if (call.isError()) {
            emit m_model->NTPChanged(m_model->nTP());
        }
    });
}

void DatetimeWork::setDatetime(const QDateTime &datetime)
{
    QDBusPendingCall call = m_timedateInter->SetNTP(false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call, datetime] {
        if (!call.isError()) {
            const QDate date = datetime.date();
            const QTime time = datetime.time();

            QDBusPendingCall call1 = m_timedateInter->SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), 0, 0);
            QDBusPendingCallWatcher *watcher1 = new QDBusPendingCallWatcher(call1, this);
            connect(watcher1, &QDBusPendingCallWatcher::finished, [this, call1] {
                if (!call1.isError()) {
                    emit m_model->systemTimeChanged();
                }
            });
        } else {
            qWarning() << "disable ntp failed : " << call.error().message();
        }
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
