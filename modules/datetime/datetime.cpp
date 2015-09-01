#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDesktopWidget>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dimagebutton.h>
#include <libdui/dapplication.h>

#include "datetime.h"
#include "timewidget.h"
#include "moduleheader.h"
#include "timezonewidget.h"
#include "timezoneitemwidget.h"
#include "dbus/dbustimedate.h"

DUI_USE_NAMESPACE

Datetime::Datetime() :
    QObject(),
    m_frame(new QFrame),
    m_dbusInter(m_frame),
    m_timezoneListWidget(new SearchList)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    // get timezone info list
    m_zoneInfoList = new QList<ZoneInfo>;
    QStringList zoneList = m_dbusInter.GetZoneList();
    for (const QString & zone : zoneList)
        m_zoneInfoList->append(m_dbusInter.GetZoneInfo(zone).argumentAt<0>());

    // sort by utc offset ascend, if utc offset is equal, sort by city.
    std::sort(m_zoneInfoList->begin(), m_zoneInfoList->end(), [this] (const ZoneInfo & z1, const ZoneInfo & z2) -> bool {
        if (z1.m_utcOffset == z2.m_utcOffset)
            return z1.m_zoneCity < z2.m_zoneCity;
        return z1.m_utcOffset < z2.m_utcOffset;
    });

    ModuleHeader *header = new ModuleHeader(tr("Date and Time"), false);

    m_dateCtrlWidget = new DateControlWidget;

    m_dateSeparator = new DSeparatorHorizontal;
    m_dateHeaderLine = new DHeaderLine;
    m_dateHeaderLine->setTitle(tr("Date"));
    m_dateHeaderLine->setContent(m_dateCtrlWidget);

    m_timezoneCtrlWidget = new TimezoneCtrlWidget;
    m_timezoneListWidget->setItemSize(310, 50);
    m_timezoneListWidget->setFixedWidth(310);

    m_timezoneHeaderLine = new DHeaderLine;
    m_timezoneHeaderLine->setTitle(tr("TimeZone"));
    m_timezoneHeaderLine->setContent(m_timezoneCtrlWidget);

    m_syncSeparator = new DSeparatorHorizontal;
    m_autoSyncSwitcher = new DSwitchButton;
    m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    m_syncHeaderLine = new DHeaderLine;
    m_syncHeaderLine->setTitle(tr("Sync Automaticly"));
    m_syncHeaderLine->setContent(m_autoSyncSwitcher);

    m_clockSeparator = new DSeparatorHorizontal;
    m_clockFormatSwitcher = new DSwitchButton;
    m_clockFormatSwitcher->setChecked(m_dbusInter.use24HourFormat());
    m_clockHeaderLine = new DHeaderLine;
    m_clockHeaderLine->setTitle(tr("Use 24-hour clock"));
    m_clockHeaderLine->setContent(m_clockFormatSwitcher);

    m_calendarSeparator = new DSeparatorHorizontal;
    m_calendar = new DCalendar(m_frame);
    m_calendar->setMinimumHeight(350);
    // if NOT zh_CN or zh_TW, hide lunar
    m_calendar->setLunarVisible(QLocale::system().name().contains("zh"));

    m_timeWidget = new TimeWidget;
    m_timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());

    showSelectedTimezoneList();

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(header);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_timeWidget);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_syncHeaderLine);
    centeralLayout->addWidget(m_syncSeparator);
    centeralLayout->addWidget(m_clockHeaderLine);
    centeralLayout->addWidget(m_clockSeparator);
    centeralLayout->addWidget(m_timezoneHeaderLine);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_timezoneListWidget);
    //centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_dateHeaderLine);
    centeralLayout->addWidget(m_dateSeparator);
    centeralLayout->addWidget(m_calendar);
    centeralLayout->addWidget(m_calendarSeparator);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_frame->setLayout(centeralLayout);
    m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());

    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::addUserTimeZone);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneCancel, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeAccept, this, &Datetime::showSelectedTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeTimezone, this, &Datetime::toRemoveTimezoneMode);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezone, this, &Datetime::showTimezoneList);
    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, &m_dbusInter, &DBusTimedate::setUse24HourFormat);
    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, m_timeWidget, &TimeWidget::setIs24HourFormat);
    connect(m_autoSyncSwitcher, &DSwitchButton::checkedChanged, this, &Datetime::switchAutoSync);
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this] () -> void {m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());});
    connect(&m_dbusInter, &DBusTimedate::TimezoneChanged, this, &Datetime::showSelectedTimezoneList);
    connect(m_timeWidget, &TimeWidget::applyTime, [this] (const QDateTime & time) -> void {
                m_dbusInter.SetTime(time.currentMSecsSinceEpoch(), true);
    });
    connect(m_dateCtrlWidget, &DateControlWidget::applyDate, [this] () -> void {
                const QDate date = m_calendar->getSelectDate();
                const QTime time = QTime::currentTime();

                m_dbusInter.SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), time.msec());
                m_calendar->resetCurrentDate(date);
    });

    qDebug() << getZoneCityListByOffset(m_dbusInter.GetZoneInfo(m_dbusInter.timezone()).argumentAt<0>().m_utcOffset);
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

const QString Datetime::getUTCOffset(int offset)
{
    const QString offsetHour = QString::number(abs(offset) / 3600);
    const QString offsetMinute = QString::number((abs(offset) % 3600) / 60);

    return std::move(QString("UTC%1%2:%3").arg(offset >= 0 ? '+' : '-')
                                          .arg(offsetHour, 2, '0')
                                          .arg(offsetMinute, 2, '0'));
}

const QString Datetime::getZoneCityListByOffset(int zoneOffset)
{
    QStringList list;
    for (const ZoneInfo & zone : *m_zoneInfoList)
        if (zone.m_utcOffset == zoneOffset)
            list.append(zone.m_zoneCity);
        else if (zone.m_utcOffset > zoneOffset)
            break;

    return std::move(list.join(", "));
}

const ZoneInfo &Datetime::getZoneInfoByName(const QString &zoneName) const
{
    for (const ZoneInfo & zone : *m_zoneInfoList)
        if (zone.m_zoneName == zoneName)
            return zone;

    qWarning() << zoneName << "not in Timezone list!!!";

    // for default
    return m_zoneInfoList->first();
}

void Datetime::showSelectedTimezoneList()
{
    m_timezoneListWidget->clear();

    m_clockHeaderLine->show();
    m_syncHeaderLine->show();
    m_dateHeaderLine->show();
    m_calendar->show();
    m_clockSeparator->show();
    m_syncSeparator->show();
    m_calendarSeparator->show();
    m_dateSeparator->show();

    const QString userZone = m_dbusInter.timezone();
    QStringList zoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;

    for (const QString & zone : zoneList)
    {
        // pass default Timezone
        if (zone == "Etc/UTC")
            continue;

        ++zoneNums;

        const ZoneInfo & zoneInfo = getZoneInfoByName(zone);
        qDebug() << zone << zoneInfo;

        TimezoneWidget *zoneWidget = new TimezoneWidget(&zoneInfo);
        zoneWidget->setZoneCities(getZoneCityListByOffset(zoneInfo.m_utcOffset));
        zoneWidget->setZoneUTCOffset(getUTCOffset(zoneInfo.m_utcOffset));
        zoneWidget->setSelected(userZone == zoneInfo.m_zoneName);

        connect(zoneWidget, &TimezoneWidget::clicked, [this, zoneWidget] () -> void {toggleTimeZone(zoneWidget);});

        m_timezoneListWidget->addItem(zoneWidget);
    }

    m_timezoneListWidget->setFixedHeight(qMin(300, 50 * zoneNums));
}

void Datetime::showTimezoneList()
{
    m_timezoneListWidget->clear();
    m_choosedZoneList.clear();

    m_clockHeaderLine->hide();
    m_syncHeaderLine->hide();
    m_dateHeaderLine->hide();
    m_calendar->hide();
    m_clockSeparator->hide();
    m_syncSeparator->hide();
    m_calendarSeparator->hide();
    m_dateSeparator->hide();

    QList<ZoneInfo> *zoneList = m_zoneInfoList;
    QStringList userZoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;
    int lastUTCOffset = -1;

    for (const ZoneInfo & zone : *zoneList)
    {
        // skip exist timezone
        if (userZoneList.contains(zone.m_zoneName))
            continue;
        // skip repeat timezone
        if (zone.m_utcOffset == lastUTCOffset)
            continue;

        lastUTCOffset = zone.m_utcOffset;
        ++zoneNums;

        TimezoneItemWidget *itemWidget = new TimezoneItemWidget(&zone);
        itemWidget->setZones(getZoneCityListByOffset(zone.m_utcOffset));
        itemWidget->setUTCOffset(getUTCOffset(zone.m_utcOffset));

        connect(itemWidget, &TimezoneItemWidget::clicked, this, &Datetime::timezoneItemChoosed);

        m_timezoneListWidget->addItem(itemWidget);
    }

    // 50 = module header + DSeparatorHorizontal
    int maxHeight = DApplication::desktop()->height();
    maxHeight -= m_timezoneHeaderLine->height() + 2 + m_timeWidget->height() + 2 + 50;

    m_timezoneListWidget->setFixedHeight(qMin(maxHeight, 50 * zoneNums));
}

void Datetime::toRemoveTimezoneMode()
{
    // update zone list.
    showSelectedTimezoneList();

    for (int i = 0; i != m_timezoneListWidget->count(); ++i)
    {
        TimezoneWidget *widget = qobject_cast<TimezoneWidget *>(m_timezoneListWidget->getItem(i)->widget());
        if (!widget)
            continue;

        widget->removeMode();
        widget->disconnect();

        connect(widget, &TimezoneWidget::clicked, [this, widget] () -> void {removeTimeZone(widget);});
    }
}

void Datetime::toggleTimeZone(TimezoneWidget *zone)
{
    if (!zone)
        return;

    qDebug() << "toggle zone: " << zone->zoneName();
    m_dbusInter.SetTimezone(zone->zoneName());
}

void Datetime::removeTimeZone(TimezoneWidget *zone)
{
    if (!zone)
        return;

    qDebug() << "remove zone: " << zone->zoneName();
    m_dbusInter.DeleteUserTimezone(zone->zoneName());
    toRemoveTimezoneMode();
}

void Datetime::addUserTimeZone()
{
    for (const QString & zone : m_choosedZoneList)
        m_dbusInter.AddUserTimezone(zone);
    m_choosedZoneList.clear();
}

void Datetime::timezoneItemChoosed(const TimezoneItemWidget *item)
{
    if (!item)
        return;

    if (!item->selected())
        m_choosedZoneList.append(item->zoneName());
    else
        m_choosedZoneList.removeOne(item->zoneName());

    m_timezoneCtrlWidget->setAcceptOrCancel(m_choosedZoneList.isEmpty());
}
