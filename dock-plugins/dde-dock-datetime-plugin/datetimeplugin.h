/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include <clockpixmap.h>
#include <dcalendar.h>
#include "dbus/dbustimedate.h"
#include <QSettings>

DWIDGET_USE_NAMESPACE

class QLabel;
class QTimer;
class DateTimePlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-datetime-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    DateTimePlugin();
    ~DateTimePlugin();

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enable) Q_DECL_OVERRIDE;
    QWidget *getItem(QString id) Q_DECL_OVERRIDE;
    QWidget *getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private slots:
    void onInitTimerTriggered();

private:
    DBusTimedate *m_dateTime = NULL;
    QString m_id = "id_datetime";
    DockPluginProxyInterface *m_proxy;

    DCalendar *m_calendar = NULL;
    QLabel *m_item = NULL;
    QTimer *m_timer = NULL;
    ClockPixmap m_clockPixmap;
    Dock::DockMode m_mode;
    bool m_use24HourFormat;

private slots:
    void updateTime();
    void onUse24HourFormatChanged();

private:
    bool showWeek();
    bool showDate();
    bool clockAnalog();
    void initSettings();
    void initCalendar();
    void setMode(Dock::DockMode mode);
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);

private:
    QSettings *m_settings = NULL;
    const int RIGHT_PADDING = 20;
};

#endif // DATETIMEPLUGIN_H
