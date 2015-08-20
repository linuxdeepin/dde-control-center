#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/dimagebutton.h>

#include "datetime.h"
#include "timewidget.h"
#include "moduleheader.h"

DUI_USE_NAMESPACE

Datetime::Datetime() :
    QObject(),
    m_frame(new QFrame),
    m_dbusInter("com.deepin.daemon.Timedate", "/com/deepin/daemon/Timedate", QDBusConnection::sessionBus())
{
    ModuleHeader *header = new ModuleHeader(tr("Date and Time"), false);

    DUI::DBaseLine *dateBaseLine = new DUI::DBaseLine;
    dateBaseLine->setLeftContent(new QLabel(tr("Date")));

    m_autoSyncSwitcher = new DUI::DSwitchButton;
    m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    DUI::DBaseLine *cyncBaseLine = new DUI::DBaseLine;
    cyncBaseLine->setLeftContent(new QLabel(tr("Sync Automaticly")));
    cyncBaseLine->setRightContent(m_autoSyncSwitcher);

    m_clockFormatSwitcher = new DUI::DSwitchButton;
    m_clockFormatSwitcher->setChecked(m_dbusInter.use24HourFormat());
    DUI::DBaseLine *clockFormat = new DUI::DBaseLine;
    clockFormat->setLeftContent(new QLabel(tr("Use 24-hour clock")));
    clockFormat->setRightContent(m_clockFormatSwitcher);

    m_calendar = new DUI::DCalendar(m_frame);
    m_calendar->setMinimumHeight(350);

    TimeWidget *timeWidget = new TimeWidget;

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(header);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(timeWidget);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(cyncBaseLine);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(clockFormat);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(dateBaseLine);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addWidget(m_calendar);
    centeralLayout->addWidget(new DUI::DSeparatorHorizontal);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_frame->setLayout(centeralLayout);

    connect(m_clockFormatSwitcher, &DUI::DSwitchButton::checkedChanged, this, &Datetime::switchClockFormat);
    connect(m_autoSyncSwitcher, &DUI::DSwitchButton::checkedChanged, this, &Datetime::switchAutoSync);
    connect(timeWidget, &TimeWidget::applyTime, [this] (const QTime & time) -> void {qDebug() << time;});
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";

    m_frame->deleteLater();
}

QFrame* Datetime::getContent()
{
    qDebug() << m_frame << "xxxxxxxxxxxxxxxx";
    return m_frame;
}

void Datetime::switchAutoSync(const bool autoSync)
{
    m_dbusInter.SetNTP(autoSync);
}

void Datetime::switchClockFormat(const bool use24HourFormat)
{
    m_dbusInter.setUse24HourFormat(use24HourFormat);
}
