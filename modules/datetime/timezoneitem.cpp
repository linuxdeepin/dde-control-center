#include "timezoneitem.h"

#include "clock.h"
#include "datetimeutil.h"
#include <QDebug>
#include <QVBoxLayout>

TimezoneItem::TimezoneItem(QFrame *parent)
    :SettingsItem(parent)
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setMargin(1);
    vlayout->setSpacing(1);
    m_city = new QLabel(tr("Wuhan"));
    m_city->setStyleSheet("QLabel{background-color:transparent}");
    m_details = new QLabel(tr("比本地快8个小时"));
    m_details->setStyleSheet("QLabel{background-color:transparent}");
    vlayout->addWidget(m_city);
    vlayout->addWidget(m_details);

    m_clock = new Clock();
    m_clock->setFixedSize(QSize(40,40));
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(15,0,15,0);
    hlayout->setSpacing(1);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(m_clock);
    hlayout->setAlignment(m_clock, Qt::AlignVCenter);
    setLayout(hlayout);
}

void TimezoneItem::setTimeZone(const QString &timezone)
{
    m_timezone = timezone;
}

void TimezoneItem::setCity(const QString &city)
{
    m_city->setText(city);
    DatetimeUtil util;

    QStringList lists = util.city2UTC(city);
    qDebug()<<lists[0];
    int del = util.hoursBetweenTwoTimeZone(lists[0]);
    QString str;
    if(del > 0)
    {
        str = QString("比本地快%1个小时").arg(del);
    }
    else
    {
        str = QString("比本地慢%1个小时").arg(-del);
    }
    setDetails(str);
    m_clock->setTimezone(lists[0]);
}

void TimezoneItem::setDetails(const QString &details)
{
    m_details->setText(details);
}
