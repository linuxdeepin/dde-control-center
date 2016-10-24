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

DWIDGET_USE_NAMESPACE

DateSettings::DateSettings(QWidget *parent)
    :QWidget(parent),
      m_clock(new Clock()),
      m_timeWidget(new TimeWidget()),
      m_yearWidget(new DateWidget(DateWidget::Year)),
      m_monthWidget(new DateWidget(DateWidget::Month))
{
    m_dayWidget = new DateWidget(DateWidget::Day);
    m_clock->setDisplay(true);
//    setStyleSheet("QWidget{background-color: rgba(25,25,26,70%) }");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setMargin(0);

    DHeaderLine* sync = new DHeaderLine();
    sync->setTitle(tr("Auto-­Sync"));

    DSwitchButton* btn = new DSwitchButton();
    sync->setContent(btn);

    layout->addWidget(sync);
    layout->addWidget(new DSeparatorHorizontal());
    layout->addWidget(m_clock);
    layout->addWidget(m_timeWidget);
    layout->setAlignment(m_timeWidget, Qt::AlignCenter);
    layout->addWidget(m_yearWidget);
    layout->addWidget(m_monthWidget);
    layout->addWidget(m_dayWidget);
//    layout->addWidget(new DSeparatorHorizontal());

    QHBoxLayout *hlayout = new QHBoxLayout();
    QPushButton* cancel = new QPushButton(tr("Cancel"));
    QPushButton* ok = new QPushButton(tr("Ok"));
    hlayout->addWidget(cancel);
//    hlayout->addWidget(new DSeparatorVertical());
    hlayout->addWidget(ok);
    layout->addLayout(hlayout);

    DHeaderLine *line = new DHeaderLine();
    line->setTitle(tr("set system timezone"));
    line->setContent(new DImageButton());

    layout->addWidget(line);

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
