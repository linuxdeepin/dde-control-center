#include "timezoneitem.h"

#include <QDebug>
#include <QVBoxLayout>

#include "datetime/datetimeutil.h"
#include "labels/normallabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

TimezoneItem::TimezoneItem(QFrame *parent)
    :SettingsItem(parent),
      m_city(new NormalLabel),
      m_details(new NormalLabel),
      m_clock(new Clock),
      m_removeBtn(new DImageButton)
{
    setFixedHeight(60);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setMargin(1);
    vlayout->setSpacing(1);

    m_city->setObjectName("DCC-Datetime-TimezoneItem-Label");
    m_details->setObjectName("DCC-Datetime-TimezoneItem-Label");

    vlayout->addWidget(m_city);
    vlayout->addWidget(m_details);

    m_removeBtn->setObjectName("DCC-Datetime-TimezoneItem-Remove");
    m_removeBtn->setFixedSize(18,18);
    m_removeBtn->setVisible(false);

    m_clock->setDrawTicks(false);
    m_clock->setFixedSize(QSize(48,48));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
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

    DatetimeUtil util;
    float del = util.hoursBetweenTwoTimeZone(info.getZoneName());

    QString str;
    if(del > 0)
    {
        str = tr("比本地快%1个小时").arg(del);
    }
    else
    {
        str = tr("比本地晚%1个小时").arg(-del);
    }

    m_details->setText(str);
    m_city->setText(info.getZoneCity());
    m_clock->setTimeZone(QTimeZone(info.getZoneName().toLatin1()));
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

}
}
