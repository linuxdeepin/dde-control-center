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

#include <libdui/dcalendar.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dseparatorhorizontal.h>

DUI_USE_NAMESPACE

class QFrame;
class Datetime: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "datetime.json")
    Q_INTERFACES(ModuleInterface)

public:
    Datetime();
    ~Datetime() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

protected:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

private:
    static const QString getUTCOffset(int offset);
    const QString getZoneCityListByOffset(int zoneOffset);
    const ZoneInfo &getZoneInfoByName(const QString &zoneName) const;
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
    QFrame *m_frame;

    DBusTimedate m_dbusInter;

    QList<ZoneInfo> *m_zoneInfoList;

    DSeparatorHorizontal *m_clockSeparator;
    DSeparatorHorizontal *m_syncSeparator;
    DSeparatorHorizontal *m_dateSeparator;
    DSeparatorHorizontal *m_calendarSeparator;
    DHeaderLine *m_clockHeaderLine;
    DHeaderLine *m_syncHeaderLine;
    DHeaderLine *m_dateHeaderLine;
    DHeaderLine *m_timezoneHeaderLine;
    DCalendar *m_calendar;
    DSwitchButton *m_clockFormatSwitcher;
    DSwitchButton *m_autoSyncSwitcher;
    DateControlWidget *m_dateCtrlWidget;
    TimezoneCtrlWidget *m_timezoneCtrlWidget;
    TimeWidget *m_timeWidget;
    SearchList *m_timezoneListWidget;
    QTimer *m_refershClockTimer;
    QSettings *m_settings;

    QList<QString> m_choosedZoneList;
};

#endif //DATETIME_H
