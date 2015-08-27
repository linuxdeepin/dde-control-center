#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dimagebutton.h>

#include "datetime.h"
#include "timewidget.h"
#include "moduleheader.h"

DUI_USE_NAMESPACE

Datetime::Datetime() :
    QObject(),
    m_frame(new QFrame),
    m_dbusInter(m_frame)
{
    ModuleHeader *header = new ModuleHeader(tr("Date and Time"), false);

    m_dateCtrlWidget = new DateControlWidget;

    DHeaderLine *dateBaseLine = new DHeaderLine;
    dateBaseLine->setTitle(tr("Date"));
    dateBaseLine->setContent(m_dateCtrlWidget);

    m_timezoneCtrlWidget = new TimezoneCtrlWidget;

    m_timezoneHeaderLine = new DHeaderLine;
    m_timezoneHeaderLine->setTitle(tr("TimeZone"));
    m_timezoneHeaderLine->setContent(m_timezoneCtrlWidget);

    m_autoSyncSwitcher = new DSwitchButton;
    m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    DHeaderLine *cyncBaseLine = new DHeaderLine;
    cyncBaseLine->setTitle(tr("Sync Automaticly"));
    cyncBaseLine->setContent(m_autoSyncSwitcher);

    m_clockFormatSwitcher = new DSwitchButton;
    m_clockFormatSwitcher->setChecked(m_dbusInter.use24HourFormat());
    DHeaderLine *clockFormat = new DHeaderLine;
    clockFormat->setTitle(tr("Use 24-hour clock"));
    clockFormat->setContent(m_clockFormatSwitcher);

    m_calendar = new DCalendar(m_frame);
    m_calendar->setMinimumHeight(350);

    TimeWidget *timeWidget = new TimeWidget;
    timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(header);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(timeWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(cyncBaseLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(clockFormat);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_timezoneHeaderLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(dateBaseLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_calendar);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_frame->setLayout(centeralLayout);

    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, &m_dbusInter, &DBusTimedate::setUse24HourFormat);
    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, timeWidget, &TimeWidget::setIs24HourFormat);
    connect(m_autoSyncSwitcher, &DSwitchButton::checkedChanged, this, &Datetime::switchAutoSync);
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this] () -> void {m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());});
    connect(timeWidget, &TimeWidget::applyTime, [this] (const QDateTime & time) -> void {
                m_dbusInter.SetTime(time.currentMSecsSinceEpoch(), true);
    });
    connect(m_dateCtrlWidget, &DateControlWidget::applyDate, [this] () -> void {
                const QDate date = m_calendar->getSelectDate();
                const QTime time = QTime::currentTime();

                m_dbusInter.SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), time.msec());
                m_calendar->resetCurrentDate(date);
    });
    //connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, [timeWidget, this] () -> void {timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());});

    qDebug() << m_dbusInter.GetZoneList().argumentAt<0>();
    m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());
    QStringList list = m_dbusInter.userTimezones();
    for (QString s : list)
    {
        qDebug() << s << " zone info: " << m_dbusInter.GetZoneInfo(s).argumentAt<0>();
    }
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";

    m_frame->setParent(nullptr);
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
