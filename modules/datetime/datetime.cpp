/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "datetime.h"
#include "timewidget.h"
#include "moduleheader.h"
#include "timezonewidget.h"
#include "timezoneitemwidget.h"
#include "dbus/dbustimedate.h"

#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QDateTime>

#include <dseparatorhorizontal.h>
#include <dimagebutton.h>
#include <dapplication.h>

DWIDGET_USE_NAMESPACE

QFrame *DatetimeModuele::getContent()
{
    qDebug() << "new Datetime begin";
    if (NULL == m_datetime) {
        m_datetime = new Datetime(this);
    }
    qDebug() << "new Datetime end";
    return m_datetime->getContent();
}

Datetime::Datetime(QObject *parent) :
    QObject(parent),
    m_frame(new QFrame()),
    m_dbusInter(this),
    m_timezoneListWidget(new SearchList),
    m_refershClockTimer(new QTimer(m_frame)),
    // config manager
    m_settings(new QSettings("deepin", "dde-control-center-datetime", this))
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Date and Time"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    loadZoneList();

    const bool isNtp = m_dbusInter.nTP();
    const bool is24HourFormat = m_dbusInter.use24HourFormat();

    m_dateCtrlWidget = new DateControlWidget;
    m_dateSeparator = new DSeparatorHorizontal;
    m_dateHeaderLine = new DHeaderLine;
    m_dateHeaderLine->setTitle(tr("Date"));
    m_dateHeaderLine->setContent(m_dateCtrlWidget);

    m_timezoneCtrlWidget = new TimezoneCtrlWidget;
    m_timezoneListWidget->setItemSize(310, 50);
    m_timezoneListWidget->setFixedWidth(310);

    m_timezoneHeaderLine = new DHeaderLine;
    m_timezoneHeaderLine->setTitle(tr("Time Zone"));
    m_timezoneHeaderLine->setContent(m_timezoneCtrlWidget);

    m_syncSeparator = new DSeparatorHorizontal;
    m_autoSyncSwitcher = new DSwitchButton;
    m_autoSyncSwitcher->setChecked(isNtp);
    m_syncHeaderLine = new DHeaderLine;
    m_syncHeaderLine->setTitle(tr("Sync Automatically"));
    m_syncHeaderLine->setContent(m_autoSyncSwitcher);

    m_clockSeparator = new DSeparatorHorizontal;
    m_clockFormatSwitcher = new DSwitchButton;
    m_clockFormatSwitcher->setChecked(is24HourFormat);
    m_clockHeaderLine = new DHeaderLine;
    m_clockHeaderLine->setTitle(tr("Use 24-hour clock"));
    m_clockHeaderLine->setContent(m_clockFormatSwitcher);

    m_calendarSeparator = new DSeparatorHorizontal;
    m_calendar = new DCalendar;
    m_calendar->setResetButtonText(tr("Reset"));
    m_calendar->setFixedHeight(330);
    m_calendar->setFixedWidth(310);
    // if NOT zh_CN or zh_TW, hide lunar
    m_calendar->setLunarVisible(QLocale::system().name().contains("zh"));

    m_timeWidget = new TimeWidget;
    m_timeWidget->setEditable(!isNtp);
    m_timeWidget->setIs24HourFormat(is24HourFormat);
    m_timeWidget->installEventFilter(this);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addWidget(new ModuleHeader(tr("Date and Time"), false));
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
//    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addWidget(m_dateHeaderLine);
    centeralLayout->addWidget(m_dateSeparator);
    centeralLayout->addWidget(m_calendar);
    centeralLayout->addWidget(m_calendarSeparator);
    centeralLayout->addStretch(1);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    m_frame->setLayout(centeralLayout);
    m_dateCtrlWidget->setVisible(!isNtp);

    connect(m_refershClockTimer, &QTimer::timeout, m_timeWidget, &TimeWidget::updateTime);
    connect(&m_dbusInter, &DBusTimedate::TimezoneChanged, m_timeWidget, &TimeWidget::updateTime);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::addUserTimeZone);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneAccept, this, &Datetime::reloadTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezoneCancel, this, &Datetime::reloadTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeAccept, this, &Datetime::reloadTimezoneList);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::removeTimezone, this, &Datetime::toRemoveTimezoneMode);
    connect(m_timezoneCtrlWidget, &TimezoneCtrlWidget::addTimezone, this, &Datetime::showTimezoneList);
    connect(m_clockFormatSwitcher, &DSwitchButton::checkedChanged, &m_dbusInter, &DBusTimedate::setUse24HourFormat);
    connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, [this] {m_timeWidget->setIs24HourFormat(m_dbusInter.use24HourFormat());});
    connect(&m_dbusInter, &DBusTimedate::Use24HourFormatChanged, m_timeWidget, &TimeWidget::updateTime);
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this] {m_timeWidget->setEditable(!m_dbusInter.nTP());});
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this]() -> void {m_dateCtrlWidget->setVisible(!m_dbusInter.nTP());});
    connect(&m_dbusInter, &DBusTimedate::TimezoneChanged, this, &Datetime::reloadTimezoneList);
    connect(m_refershClockTimer, &QTimer::timeout, m_timeWidget, [this] {
        if (QDate::currentDate() != m_calendar->getCurrentDate())
            m_calendar->setCurrentDate(QDate::currentDate());
    });
    connect(m_autoSyncSwitcher, &DSwitchButton::checkedChanged, [this] {
        m_autoSyncSwitcher->blockSignals(true);
        QDBusPendingReply<> reply = m_dbusInter.SetNTP(m_autoSyncSwitcher->checked());
        reply.waitForFinished();
        if (reply.isError())
            m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
        m_autoSyncSwitcher->blockSignals(false);
    });
    connect(&m_dbusInter, &DBusTimedate::NTPChanged, [this]{
        m_autoSyncSwitcher->setChecked(m_dbusInter.nTP());
    });

    connect(m_timeWidget, &TimeWidget::applyTime, [this](const QDateTime & time) -> void {
        qDebug() << "set time: " << time << time.currentMSecsSinceEpoch();
        m_dbusInter.SetDate(time.date().year(), time.date().month(), time.date().day(), time.time().hour(), time.time().minute(), time.time().second(), time.time().msec()).waitForFinished();
    });
    connect(m_dateCtrlWidget, &DateControlWidget::applyDate, [this]() -> void {
        const QDate date = m_calendar->getSelectDate();
        const QTime time = QTime::currentTime();
        qDebug() << "set date: " << date << time;

        QDBusPendingReply<> reply = m_dbusInter.SetDate(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), time.msec());
        reply.waitForFinished();
        if (!reply.isError())
            m_calendar->setCurrentDate(date);
    });

    loadTimezoneList();
    m_refershClockTimer->setInterval(1 * 1000);
    m_refershClockTimer->start();
    m_frame->installEventFilter(this);
}

Datetime::~Datetime()
{
    qDebug() << "~Datetime";
}

QFrame *Datetime::getContent()
{
    return m_frame;
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
    for (const ZoneInfo &zone : *m_zoneInfoList)
        if (zone.getUTCOffset() == zoneOffset)
            if (zone.getZoneCity() == "Asia/Shanghai") {
                list.append("Beijing");
            } else {
                list.append(zone.getZoneCity());
            }
        else if (zone.getUTCOffset() > zoneOffset) {
            break;
        }

    return std::move(list.join(", "));
}

const ZoneInfo &Datetime::getZoneInfoByName(const QString &zoneName)
{
    const ZoneInfo info = m_dbusInter.GetZoneInfo(zoneName);

    for (const ZoneInfo &zone : *m_zoneInfoList)
        if (zone.getUTCOffset() == info.getUTCOffset()) {
            return zone;
        }

    qWarning() << zoneName << "not in Timezone list!!!";

    // for default
    return m_zoneInfoList->first();
}

void Datetime::loadTimezoneList()
{
    const QString userZone = m_dbusInter.timezone();
    const int userZoneOffset = getZoneInfoByName(userZone).getUTCOffset();
    QStringList zoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;
    // pass duplicate time zone
    QList<int> selectedOffset;

    for (const QString &zone : zoneList) {
        const ZoneInfo &zoneInfo = getZoneInfoByName(zone);
        if (selectedOffset.contains(zoneInfo.getUTCOffset())) {
            continue;
        }
        selectedOffset.append(zoneInfo.getUTCOffset());

        ++zoneNums;

        TimezoneWidget *zoneWidget = new TimezoneWidget(&zoneInfo);
        zoneWidget->setZoneCities(getZoneCityListByOffset(zoneInfo.getUTCOffset()));
        zoneWidget->setZoneUTCOffset(getUTCOffset(zoneInfo.getUTCOffset()));
        zoneWidget->setSelected(userZoneOffset == zoneInfo.getUTCOffset());

        connect(zoneWidget, &TimezoneWidget::clicked, [this, zoneWidget]() -> void {toggleTimeZone(zoneWidget);});

        m_timezoneListWidget->addItem(zoneWidget);
    }

    adjustItemHeight();

    // when list item count <= 1, control widget should hide "delete" button
    m_timezoneCtrlWidget->setListNums(zoneNums);
}

void Datetime::reloadTimezoneList()
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

    loadTimezoneList();
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

    QStringList userZoneList = m_dbusInter.userTimezones();
    int zoneNums = 0;
    int lastUTCOffset = -1;

    // m_zoneInfoList is sorted list.
    for (const ZoneInfo &zone : *m_zoneInfoList) {
        // skip repeat timezone
        if (zone.getUTCOffset() == lastUTCOffset) {
            continue;
        }
        lastUTCOffset = zone.getUTCOffset();

        // skip exist timezone
        if (userZoneList.contains(zone.getZoneName())) {
            continue;
        }

        ++zoneNums;

        TimezoneItemWidget *itemWidget = new TimezoneItemWidget(&zone);
        itemWidget->setZones(getZoneCityListByOffset(zone.getUTCOffset()));
        itemWidget->setUTCOffset(getUTCOffset(zone.getUTCOffset()));

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
    reloadTimezoneList();

    for (int i = 0; i != m_timezoneListWidget->count(); ++i) {
        TimezoneWidget *widget = qobject_cast<TimezoneWidget *>(m_timezoneListWidget->getItem(i)->widget());
        if (!widget) {
            continue;
        }

        widget->removeMode();
        widget->disconnect();

        connect(widget, &TimezoneWidget::clicked, [this, widget]() -> void {removeTimeZone(widget);});
    }
}

void Datetime::adjustItemHeight()
{
    // m_timezoneListWidget height
    int maxHeight = DApplication::desktop()->height();
    maxHeight -= m_timezoneHeaderLine->height() + 2 + m_timeWidget->height() + 2;
    maxHeight -= (m_syncHeaderLine->height() + 2) * 4 + m_calendar->height() + 13;

    int min = qMin(maxHeight, 50 * m_timezoneListWidget->count());

    m_timezoneListWidget->setFixedHeight(qMax(min, 50));
}

bool Datetime::eventFilter(QObject *o, QEvent *e)
{
    if ((o == m_frame || o == m_timeWidget) &&
            e->type() == QEvent::Resize) {
        adjustItemHeight();
    }

    return false;
}

void Datetime::toggleTimeZone(TimezoneWidget *zone)
{
    if (!zone) {
        return;
    }

    qDebug() << "toggle zone: " << zone->zoneName();
    m_dbusInter.SetTimezone(zone->zoneName()).waitForFinished();
}

void Datetime::removeTimeZone(TimezoneWidget *zone)
{
    if (!zone) {
        return;
    }

    qDebug() << "remove zone: " << zone->zoneName();
//    m_dbusInter.DeleteUserTimezone(zone->zoneName()).waitForFinished();

    // we need to ensure zone name is in the user zone list.
    const ZoneInfo info = getZoneInfoByName(zone->zoneName());
    for (const QString zone : m_dbusInter.userTimezones())
    {
        if (getZoneInfoByName(zone).getUTCOffset() == info.getUTCOffset())
        {
            m_dbusInter.DeleteUserTimezone(zone).waitForFinished();
            break;
        }
    }

    //toRemoveTimezoneMode();
    const int i = m_timezoneListWidget->indexOf(zone);
    m_timezoneListWidget->removeItem(i);
    adjustItemHeight();
}

void Datetime::addUserTimeZone()
{
    qDebug() << "add zone list: " << m_choosedZoneList;
    for (const QString &zone : m_choosedZoneList) {
        m_dbusInter.AddUserTimezone(zone).waitForFinished();
    }
    m_choosedZoneList.clear();
}

void Datetime::timezoneItemChoosed()
{
    const TimezoneItemWidget *item = qobject_cast<TimezoneItemWidget *>(sender());

    if (item) {
        if (item->selected()) {
            m_choosedZoneList.append(item->zoneName());
        } else {
            m_choosedZoneList.removeOne(item->zoneName());
        }
    }

    m_timezoneCtrlWidget->setAcceptOrCancel(m_choosedZoneList.isEmpty());
}

void Datetime::loadZoneList()
{
    const QString zoneInfoListKey = "zoneInfoList";

    // get timezone info list
    m_zoneInfoList = new QList<ZoneInfo>;

    if (m_settings->value("version").toString() == qApp->applicationVersion()) {
        // try to read config file
        QByteArray bytes = m_settings->value(zoneInfoListKey).toByteArray();
        QDataStream readStream(&bytes, QIODevice::ReadOnly);
        readStream >> *m_zoneInfoList;

        // FIXME: 读文件有时会出现由于文件错误而导致数据不正常，所以简单验证一下数据
        if (!m_zoneInfoList->isEmpty() &&
                !m_zoneInfoList->first().getZoneName().isEmpty() &&
                !m_zoneInfoList->first().getZoneCity().isEmpty() &&
                abs(m_zoneInfoList->first().getUTCOffset()) <= 3600 * 24) {
            return;
        }
    }

    qDebug() << "load zoneInfoList from d-bus";

    // load config file error, read from d-bus
    QDBusPendingReply<QStringList> list = m_dbusInter.GetZoneList();
    list.waitForFinished();
    QStringList zoneList = list.value();

    for (const QString &zone : zoneList) {
        QDBusPendingReply<ZoneInfo> info;
        if (zone == "Asia/Beijing") {
            info = m_dbusInter.GetZoneInfo("Asia/Shanghai");
        } else {
            info = m_dbusInter.GetZoneInfo(zone);
        }

        info.waitForFinished();
        m_zoneInfoList->append(info.argumentAt<0>());
    }

    // sort by utc offset ascend, if utc offset is equal, sort by city.
    std::sort(m_zoneInfoList->begin(), m_zoneInfoList->end(), [this](const ZoneInfo & z1, const ZoneInfo & z2) -> bool {
        if (z1.getUTCOffset() == z2.getUTCOffset())
            return z1.getZoneCity() < z2.getZoneCity();
        return z1.getUTCOffset() < z2.getUTCOffset();
    });

    QByteArray writeBytes;
    QDataStream writeStream(&writeBytes, QIODevice::WriteOnly);
    writeStream << *m_zoneInfoList;

    m_settings->setValue(zoneInfoListKey, writeBytes);
    m_settings->setValue("version", qApp->applicationVersion());
}
