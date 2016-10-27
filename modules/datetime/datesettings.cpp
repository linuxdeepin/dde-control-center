#include "datesettings.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <dseparatorhorizontal.h>
#include <dseparatorvertical.h>
#include <dheaderline.h>
#include <dimagebutton.h>
#include <dswitchbutton.h>
#include <QDate>

#include "settingsgroup.h"
#include "settingsitem.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "nextpagewidget.h"

DWIDGET_USE_NAMESPACE
using namespace dcc;

DateSettings::DateSettings(QWidget *parent)
    :QWidget(parent),
      m_clock(new Clock()),
      m_timeWidget(new TimeWidget()),
      m_yearWidget(new DateWidget(DateWidget::Year)),
      m_monthWidget(new DateWidget(DateWidget::Month)),
      m_dayWidget(new DateWidget(DateWidget::Day))
{
    m_clock->setDisplay(true);
//    setStyleSheet("QWidget{background-color: rgba(25,25,26,70%) }");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setMargin(0);

    SettingsGroup* autosyncGroup = new SettingsGroup();
    SwitchWidget* autoItem = new SwitchWidget();
    autoItem->setTitle(tr("Auto-­Sync"));
    autosyncGroup->appendItem(autoItem);

    SettingsGroup* datetimeGroup = new SettingsGroup();
    datetimeGroup->appendItem(m_clock);
    datetimeGroup->appendItem(m_timeWidget);
    datetimeGroup->appendItem(m_yearWidget);
    datetimeGroup->appendItem(m_monthWidget);
    datetimeGroup->appendItem(m_dayWidget);

    SettingsGroup* timezoneGroup = new SettingsGroup();
    NextPageWidget* timezoneItem = new NextPageWidget();
    timezoneItem->setTitle(tr("Change System Timezone"));
    timezoneItem->setValue(tr("UTC+8:00"));
    timezoneGroup->appendItem(timezoneItem);

    layout->addWidget(autosyncGroup);
    layout->addWidget(datetimeGroup);
    layout->addWidget(timezoneGroup);

    connect(m_yearWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
    connect(m_monthWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
    connect(m_dayWidget, SIGNAL(dataChanged(DateWidget::Type,int)), this, SLOT(slotMonthChange(DateWidget::Type, int)));
}

void DateSettings::slotMonthChange(DateWidget::Type type, int data)
{
    if(type == DateWidget::Year)
    {
        QDate date(data, m_monthWidget->dateString().toInt(), 1);
        int day = date.daysInMonth();
        m_dayWidget->setMax(day);
    }
    else if(type == DateWidget::Month)
    {
        QDate date(m_yearWidget->dateString().toInt(), data, 1);
        int day = date.daysInMonth();
        m_dayWidget->setMax(day);
    }
    else
    {
//        QDate date(m_monthWidget->dateString().toInt(), data, m_dayWidget->dateString().toInt());
//        int day = date.daysInMonth();
//        m_dayWidget->setMax(day);
    }
}
