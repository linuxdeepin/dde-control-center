#include "datetimework.h"
#include <QDebug>

DatetimeWork::DatetimeWork(DatetimeModel *model, QObject *parent)
    : QObject(parent),
      m_model(model)
{
    m_timedateInter = new Timedate("com.deepin.daemon.Timedate",
                                   "/com/deepin/daemon/Timedate",
                                   QDBusConnection::sessionBus(), this);

    connect(m_timedateInter, SIGNAL(TimezoneChanged(QString)), m_model, SIGNAL(timezoneChanged(QString)));
    connect(m_timedateInter, SIGNAL(NTPChanged(bool)), m_model, SLOT(setNTP(bool)));
    connect(this, SIGNAL(NTPChanged(bool)), m_model, SLOT(setNTP(bool)));

    m_model->setNTP(m_timedateInter->nTP());

    m_timedateInter->setSync(false);
}

void DatetimeWork::setTimezone(const QString &timezone)
{
    QDBusPendingReply<> reply = m_timedateInter->SetTimezone(timezone);
    reply.waitForFinished();
}

void DatetimeWork::setDatetime(int year, int month, int day, int hour, int minute)
{
    m_timedateInter->SetDate(year,month,day, hour, minute, 0, 0);
}

void DatetimeWork::setNTP(bool ntp)
{
    QDBusPendingReply<> reply = m_timedateInter->SetNTP(ntp);
    reply.waitForFinished();
    if(reply.isError())
    {
        emit NTPChanged(m_timedateInter->nTP());
    }
}

void DatetimeWork::onProperty(const QString &propName, const QVariant &value)
{
    qDebug()<<Q_FUNC_INFO<<propName<<value;
}
