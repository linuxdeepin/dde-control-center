#include "timezoneitem.h"

#include <QDebug>
#include <QVBoxLayout>

#include "labels/normallabel.h"
#include "labels/tipslabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimezoneItem::TimezoneItem(QFrame *parent)
    :SettingsItem(parent),
      m_city(new NormalLabel),
      m_details(new TipsLabel),
      m_clock(new Clock),
      m_removeBtn(new DImageButton)
{
    setFixedHeight(60);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    m_city->setObjectName("DCC-Datetime-TimezoneItem-Label");
    m_details->setObjectName("DCC-Datetime-TimezoneItem-Label");

    vlayout->addStretch();
    vlayout->addWidget(m_city);
    vlayout->addSpacing(1);
    vlayout->addWidget(m_details);
    vlayout->addStretch();

    m_removeBtn->setObjectName("DCC-Datetime-TimezoneItem-Remove");
    m_removeBtn->setFixedSize(18,18);
    m_removeBtn->setVisible(false);

    m_clock->setDrawTicks(false);
    m_clock->setFixedSize(QSize(48,48));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->setContentsMargins(20, 0, 10, 0);
    hlayout->addLayout(vlayout);
    hlayout->addStretch();
    hlayout->addWidget(m_clock, 0, Qt::AlignVCenter);
    hlayout->addWidget(m_removeBtn);
    setLayout(hlayout);

    connect(m_removeBtn, &DImageButton::clicked, this, &TimezoneItem::removeClicked);
}

void TimezoneItem::setTimeZone(const ZoneInfo &info)
{
    m_timezone = info;

    updateInfo();
}

void TimezoneItem::toRemoveMode()
{
    m_clock->setVisible(false);
    m_removeBtn->setVisible(true);
}

void TimezoneItem::toNormalMode()
{
    m_removeBtn->setVisible(false);;
    m_clock->setVisible(true);
}

void TimezoneItem::updateInfo()
{
    const QTimeZone localZone( QTimeZone::systemTimeZone() );
    const QDateTime localTime( QDateTime::currentDateTime() );
    const QTimeZone zone(m_timezone.getZoneName().toLatin1());
    const QDateTime utcTime( QDateTime::currentDateTimeUtc() );

    const int timeDelta = (zone.offsetFromUtc(utcTime) - localZone.offsetFromUtc(utcTime)) / 3600;

    QString dateLiteral;
    if (localTime.time().hour() + timeDelta >= 24) {
        dateLiteral = tr("Tomorrow");
    } else if (localTime.time().hour() + timeDelta <= 0) {
        dateLiteral = tr("Yesterday");
    } else {
        dateLiteral = tr("Today");
    }

    QString compareLiteral;
    if(timeDelta > 0) {
        compareLiteral = tr("%1 hours earlier than local").arg(timeDelta);
    } else {
        compareLiteral = tr("%1 hours late than local").arg(-timeDelta);
    }

    m_details->setText(QString("%1, %2").arg(dateLiteral).arg(compareLiteral));
    m_city->setText(m_timezone.getZoneCity());
    m_clock->setTimeZone(QTimeZone(m_timezone.getZoneName().toLatin1()));
}

}
}
