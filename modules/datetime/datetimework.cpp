#include "datetimework.h"
#include <QDebug>

DatetimeWork::DatetimeWork(QObject *parent)
    : QObject(parent)
{
    m_timedateInter = new Timedate("com.deepin.daemon.Timedate",
                                   "/com/deepin/daemon/Timedate",
                                   QDBusConnection::sessionBus(), this);
    connect(m_timedateInter,SIGNAL(NTPChanged(bool)), this, SIGNAL(NTPChanged(bool)));

    m_timedateInter->setSync(false);
}

bool DatetimeWork::nTP() const
{
    bool ntp = m_timedateInter->nTP();

    return ntp;
}

void DatetimeWork::setDatetime(int year, int month, int day, int hour, int minute)
{
    m_timedateInter->SetDate(year,month,day, hour, minute, 0, 0);
}

void DatetimeWork::setNTP(bool ntp)
{
    QDBusPendingReply<> reply = m_timedateInter->SetNTP(ntp);
    reply.waitForFinished();

    if (reply.isError())
    {
        emit NTPChanged(m_timedateInter->nTP());
    }
}

void DatetimeWork::onProperty(const QString &propName, const QVariant &value)
{
    qDebug()<<Q_FUNC_INFO<<propName<<value;
}
