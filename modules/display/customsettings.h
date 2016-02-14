/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CUSTOMSETTINGS_H
#define CUSTOMSETTINGS_H

#include <QFrame>
#include <QMap>

#include <libdui/dbuttongrid.h>
#include <libdui/dslider.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbaseline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dtextbutton.h>

#include "dbus/monitorinterface.h"
#include "dbus/displayinterface.h"

DUI_USE_NAMESPACE

class Monitor;
class MonitorGround;
class CustomSettings : public QFrame
{
    Q_OBJECT
public:
    explicit CustomSettings(DisplayInterface *display, MonitorGround *monitorGround,
                            const QList<MonitorInterface *> &list,
                            QWidget *parent = 0);
    ~CustomSettings();

    void updateUI(const QList<MonitorInterface *> &list);

private:
    QList<MonitorInterface *> m_dbusMonitors;
    DisplayInterface *m_dbusDisplay;
    QMap<ushort, QString> m_rotationMap;
    QStringList m_monitorNameList;
    DBaseLine *m_brightnessHeaderLine;
    DSeparatorHorizontal *m_brightnessLineSeparator;
    DArrowLineExpand *m_brightnessExpand;
    QPointer<DArrowLineExpand> m_primaryMonitor;
    QVBoxLayout *m_mainLayout;
    DTextButton *m_cancelButton = new DTextButton(tr("Cancel"));
    DTextButton *m_applyButton = new DTextButton(tr("Confirm"));
    MonitorGround *m_monitorGround;
    bool m_ignoreSliderChang = false;

    QMap<QString, QStringList> m_mapNameToRotationLabels;
    QMap<QString, QStringList> m_mapNameToResolutionLabels;

    QStringList getResolutionLabels(MonitorInterface *dbus);
    QStringList getRotationLabels(MonitorInterface *dbus);
    DSlider *getBrightnessSlider(const QString &name);

signals:
    void cancel();

private slots:
    void updateResolutionButtons(MonitorInterface *dbus, DButtonGrid *resolutionButtons);
    void updateRotationButtons(MonitorInterface *dbus, DButtonGrid *rotationButtons);
    void updateBrightnessSlider(const QString &name, DSlider *brightnessSlider);
    void updateBrightnessLayout();
    void onHasChangedChanged();
};

#endif // CUSTOMSETTINGS_H
