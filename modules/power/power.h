/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QtPlugin>
#include <QLabel>

#include <dtextbutton.h>
#include <dheaderline.h>
#include <dbaseline.h>
#include <dseparatorhorizontal.h>
#include <dslider.h>
#include <dsegmentedcontrol.h>
#include <dswitchbutton.h>
#include <dbuttongrid.h>
#include <dbaseexpand.h>

#include "interfaces.h"
#include "dbus/dbuspower.h"
#include "powermanagement.h"
#include "presspowerbuttonaction.h"
#include "dbus/dbuspower.h"
#include "dynamiclabel.h"
#include "dbreathinglabel.h"
#include "widgets/doublecornerwidget.h"
#include "widgets/powerswitchwidget.h"
#include "widgets/choosedelaytimewidget.h"

#include "powerinterface.h"

DWIDGET_USE_NAMESPACE

class QLabel;
class QFrame;
class PowerModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "power.json")
    Q_INTERFACES(ModuleInterface)

public:
    QFrame *getContent() Q_DECL_OVERRIDE;
};

class Power: public QObject {
    Q_OBJECT

public:
    Power();
    ~Power();

public slots:
    QFrame *getContent();

    void handleScreenBlackDelayChanged();
    void handleSleepDelayChanged();
    void handleScreenBlackLockChanged();
    void handleSleepLockChanged();
    //handle battery state changed including process the signals:
    // batteryStateChanged, batteryPercentageChanged, onBatteryChanged.
    void handleBatteryStateChanged();
    //handleLidActionChanged including process the signals:
    //lidIsPresentChanged lidClosedActionChanged.
    void handleLidActionChanged();

private:
    QString m_displayLightUpTime;
    QString m_suspendTime;
    QFrame *m_frame;
    //dbus backend
    PowerInterface* m_powerInterface;
    PowerManagement* m_powerTitleManagement;

    ChooseDelayTimeWidget* m_screenBlackTimeWidget;
    ChooseDelayTimeWidget* m_sleepTimeWidget;
    //separatorHorizontal line
    DSeparatorHorizontal *dsHorizontalFirst;
    DSeparatorHorizontal *dsHorizontalSecond;
    DSeparatorHorizontal *dsHorizontalThird;
    DSeparatorHorizontal *dsHorizontalFourth;

    PowerSwitchWidget* m_screenBlackSwitchWidget;
    PowerSwitchWidget* m_sleepSwitchWidget;
    PowerSwitchWidget* m_lidCloseSwitchWidget;

    void initUI();
    void updateUI();
    void initConnect();


};
#endif
