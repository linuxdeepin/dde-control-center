#include "timezoneitem.h"

#include "datetime/datetimeutil.h"
#include <QDebug>
#include <QVBoxLayout>

using namespace dcc;

TimezoneItem::TimezoneItem(QFrame *parent)
    :SettingsItem(parent)
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setMargin(1);
    vlayout->setSpacing(1);
    m_city = new QLabel();
    m_city->setStyleSheet("QLabel{background-color:transparent}");
    m_details = new QLabel();
    m_details->setStyleSheet("QLabel{background-color:transparent}");
    vlayout->addWidget(m_city);
    vlayout->addWidget(m_details);

    m_removeBtn = new DImageButton(":/icon/list_delete_normal.png",
                                   ":/icon/list_delete_hover.png",
                                   ":/icon/list_delete_press.png");
    m_removeBtn->setFixedSize(18,18);

    m_back = new QFrame();
    m_back->setFixedSize(40,40);
    QVBoxLayout* removeLayout = new QVBoxLayout();
    removeLayout->addWidget(m_removeBtn);
    removeLayout->setAlignment(m_removeBtn, Qt::AlignCenter);
    m_back->setLayout(removeLayout);
    m_back->hide();

    m_clock = new Clock();
    m_clock->setFixedSize(QSize(40,40));
    QHBoxLayout* hlayout = new QHBoxLayout();
    m_layout = hlayout;
    hlayout->setContentsMargins(15,0,15,0);
    hlayout->setSpacing(1);
    hlayout->addLayout(vlayout);
    hlayout->addStretch();
    hlayout->addWidget(m_back);
    hlayout->addWidget(m_clock);
    hlayout->setAlignment(m_removeBtn, Qt::AlignVCenter);
    setLayout(hlayout);

    connect(m_removeBtn, SIGNAL(clicked()), this, SLOT(slotRemoveSelf()));
}

void TimezoneItem::setTimeZone(const QString &timezone)
{
    m_timezone = timezone;
}

void TimezoneItem::setCity(const Timezone &tz)
{
    m_curTimezone = tz;
    m_city->setText(tz.m_city);
    DatetimeUtil util;

    float del = util.hoursBetweenTwoTimeZone(tz.m_timezone);
    QString str;
    if(del > 0)
    {
        str = QString("比本地快%1个小时").arg(del);
    }
    else
    {
        str = QString("比本地晚%1个小时").arg(-del);
    }
    setDetails(str);
    m_clock->setTimezone(tz.m_timezone);
}

void TimezoneItem::setDetails(const QString &details)
{
    m_details->setText(details);
}

void TimezoneItem::slotStatus(bool flags)
{
    if(flags)
    {
        toRemoveMode();
    }
    else
    {
        toNormalMode();
    }
}

void TimezoneItem::slotRemoveSelf()
{
    emit destroySelf();
    emit removeTimezone(m_curTimezone);
    this->deleteLater();
}

void TimezoneItem::toRemoveMode()
{
    m_clock->setVisible(false);
    m_back->setVisible(true);
}

void TimezoneItem::toNormalMode()
{
    m_back->setVisible(false);;
    m_clock->setVisible(true);
}
