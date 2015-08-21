#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dheaderline.h>
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

    DUI::DHeaderLine *dateBaseLine = new DUI::DHeaderLine;
    dateBaseLine->setTitle(tr("Date"));

    m_autoSyncSwitcher = new DUI::DSwitchButton;
    m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    DUI::DHeaderLine *cyncBaseLine = new DUI::DHeaderLine;
    cyncBaseLine->setTitle(tr("Sync Automaticly"));
    cyncBaseLine->setContent(m_autoSyncSwitcher);

    m_clockFormatSwitcher = new DUI::DSwitchButton;
    m_clockFormatSwitcher->setChecked(m_dbusInter.use24HourFormat());
    DUI::DHeaderLine *clockFormat = new DUI::DHeaderLine;
    clockFormat->setTitle(tr("Use 24-hour clock"));
    clockFormat->setContent(m_clockFormatSwitcher);

    m_calendar = new DUI::DCalendar(m_frame);
    m_calendar->setMinimumHeight(350);

    TimeWidget *timeWidget = new TimeWidget;
    timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());

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

    connect(m_clockFormatSwitcher, &DUI::DSwitchButton::checkedChanged, &m_dbusInter, &DBusTimedate::setUse24HourFormat);
    connect(m_clockFormatSwitcher, &DUI::DSwitchButton::checkedChanged, timeWidget, &TimeWidget::setIs24HourFormat);
    connect(m_autoSyncSwitcher, &DUI::DSwitchButton::checkedChanged, this, &Datetime::switchAutoSync);
    connect(timeWidget, &TimeWidget::applyTime, [this] (const QDateTime & time) -> void {
        m_dbusInter.SetTime(time.currentMSecsSinceEpoch(), true);
    });
    //connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, [timeWidget, this] () -> void {timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());});

    qDebug() << m_dbusInter.use24HourFormat();
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";

    m_frame->deleteLater();
}

QFrame* Datetime::getContent()
{
    return m_frame;
}

void Datetime::switchAutoSync(const bool autoSync)
{
    m_dbusInter.SetNTP(autoSync);
}
