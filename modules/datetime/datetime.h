/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>
#include <QtPlugin>
#include <QSettings>

#include "interfaces.h"
#include "datecontrolwidget.h"
#include "timezonectrlwidget.h"
#include "timezonewidget.h"
#include "timewidget.h"
#include "timezoneitemwidget.h"
#include "dbus/dbustimedate.h"

#include "searchlist.h"

#include <dcalendar.h>
#include <dswitchbutton.h>
#include <dheaderline.h>
#include <dseparatorhorizontal.h>

DWIDGET_USE_NAMESPACE

class QFrame;

class Datetime: public QObject
{
    Q_OBJECT

public:
    Datetime(QObject *parent = NULL);
    ~Datetime();
    QFrame *getContent();

protected:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

private:
    static const QString getUTCOffset(int offset);
    const QString getZoneCityListByOffset(int zoneOffset);
    const ZoneInfo &getZoneInfoByName(const QString &zoneName);
    void loadTimezoneList();
    void reloadTimezoneList();
    void showTimezoneList();
    void toRemoveTimezoneMode();
    void adjustItemHeight();

private slots:
    void toggleTimeZone(TimezoneWidget *zone);
    void removeTimeZone(TimezoneWidget *zone);
    void addUserTimeZone();
    void timezoneItemChoosed();
    void loadZoneList();

private:
    QFrame *m_frame = NULL;

    DBusTimedate m_dbusInter;

    QList<ZoneInfo> *m_zoneInfoList = NULL;

    DSeparatorHorizontal *m_clockSeparator = NULL;
    DSeparatorHorizontal *m_syncSeparator = NULL;
    DSeparatorHorizontal *m_dateSeparator = NULL;
    DSeparatorHorizontal *m_calendarSeparator = NULL;
    DHeaderLine *m_clockHeaderLine = NULL;
    DHeaderLine *m_syncHeaderLine = NULL;
    DHeaderLine *m_dateHeaderLine = NULL;
    DHeaderLine *m_timezoneHeaderLine = NULL;
    DCalendar *m_calendar = NULL;
    DSwitchButton *m_clockFormatSwitcher = NULL;
    DSwitchButton *m_autoSyncSwitcher = NULL;
    DateControlWidget *m_dateCtrlWidget = NULL;
    TimezoneCtrlWidget *m_timezoneCtrlWidget = NULL;
    TimeWidget *m_timeWidget = NULL;
    SearchList *m_timezoneListWidget = NULL;
    QTimer *m_refershClockTimer = NULL;
    QSettings *m_settings = NULL;

    QList<QString> m_choosedZoneList;
};

class DatetimeModuele: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "datetime.json")
    Q_INTERFACES(ModuleInterface)

public:
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    Datetime *m_datetime = NULL;
};


#endif //DATETIME_H
