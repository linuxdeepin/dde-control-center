#include "datesettings.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <dheaderline.h>
#include <dimagebutton.h>
#include <dswitchbutton.h>
#include <QDate>

#include "settingsgroup.h"
#include "settingsitem.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "nextpagewidget.h"
#include "translucentframe.h"

DWIDGET_USE_NAMESPACE
using namespace dcc;

namespace dcc {
namespace datetime {

DateSettings::DateSettings(QWidget *parent)
    :ContentWidget(parent),
      m_clock(new Clock()),
      m_timeWidget(new TimeWidget()),
      m_dayWidget(new DateWidget(DateWidget::Day)),
      m_yearWidget(new DateWidget(DateWidget::Year)),
      m_monthWidget(new DateWidget(DateWidget::Month))
{
    setTitle(tr("Date time details"));
    m_clock->setDisplay(true);

    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(1);
    layout->setMargin(0);

    SettingsGroup* autosyncGroup = new SettingsGroup();
    m_autoItem = new SwitchWidget();
    m_autoItem->setTitle(tr("Auto-­Sync"));
    autosyncGroup->appendItem(m_autoItem);

    SettingsGroup* datetimeGroup = new SettingsGroup();
    datetimeGroup->appendItem(m_clock);
    datetimeGroup->appendItem(m_timeWidget);
    datetimeGroup->appendItem(m_yearWidget);
    datetimeGroup->appendItem(m_monthWidget);
    datetimeGroup->appendItem(m_dayWidget);

    m_okGroup = new SettingsGroup();
    m_timeBtn = new TimeButton();
    m_okGroup->appendItem(m_timeBtn);

    SettingsGroup* timezoneGroup = new SettingsGroup();
    m_timezoneItem = new NextPageWidget();
    m_timezoneItem->setTitle(tr("Change System Timezone"));
    m_timezoneItem->setValue(QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName));
    timezoneGroup->appendItem(m_timezoneItem);

    layout->addWidget(autosyncGroup);
    layout->addWidget(datetimeGroup);
    layout->addSpacerItem(new QSpacerItem(300,10));
    layout->addWidget(m_okGroup);
    layout->addSpacerItem(new QSpacerItem(300,20));
    layout->addWidget(timezoneGroup);

    setContent(widget);

    connect(m_timeBtn, SIGNAL(confirm()), this, SLOT(slotConfirm()));
    connect(m_timeBtn, SIGNAL(cancel()), this, SIGNAL(back()));
    connect(m_autoItem, SIGNAL(checkedChanegd(const bool)), this, SIGNAL(autoSyncChanged(const bool)));
//    connect(autoItem, SIGNAL(checkedChanegd(const bool)), this, SLOT(slotAutoSync(bool)));
    connect(m_yearWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
    connect(m_monthWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
    connect(m_dayWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
    connect(m_timezoneItem, SIGNAL(clicked()), this, SIGNAL(changeClick()));
}

void DateSettings::setNTP(bool ntp)
{
    slotAutoSync(ntp);
}

void DateSettings::setTimezone(const QString &timezone)
{
    QTimeZone tz(timezone.toStdString().c_str());
    m_timezoneItem->setValue(tz.utc().id());
    m_clock->setTimezone(timezone);
}

void DateSettings::slotMonthChange(DateWidget::Type type, int data)
{
    if(type == DateWidget::Year)
    {
        QDate date(data, m_monthWidget->data(), 1);
        int day = date.daysInMonth();
        m_dayWidget->setMax(day);
    }
    else if(type == DateWidget::Month)
    {
        QDate date(m_yearWidget->data(), data, 1);
        int day = date.daysInMonth();
        m_dayWidget->setMax(day);
    }
    else
    {
//        QDate date(m_monthWidget->data(), data, m_dayWidget->data());
//        int day = date.daysInMonth();
//        m_dayWidget->setMax(day);
    }
}

void DateSettings::slotConfirm()
{
    int year = m_yearWidget->data();
    int month = m_monthWidget->data();
    int day = m_dayWidget->data();

    int hour = m_timeWidget->hour();
    int minute = m_timeWidget->minute();

    emit dateChanged(year, month, day, hour, minute);
}

void DateSettings::slotCancel()
{
}

void DateSettings::slotAutoSync(const bool checked)
{
    m_autoItem->setChecked(checked);

    if(checked)
    {
        m_timeWidget->hide();
        m_yearWidget->hide();
        m_monthWidget->hide();
        m_dayWidget->hide();
        m_okGroup->hide();
    }
    else
    {
        m_timeWidget->show();
        m_yearWidget->show();
        m_monthWidget->show();
        m_dayWidget->show();
        m_okGroup->show();
    }
}
}
}
