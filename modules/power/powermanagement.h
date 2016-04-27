/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef POWERMANAGEMENT
#define POWERMANAGEMENT

#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

#include <dtextbutton.h>
#include <dseparatorhorizontal.h>
#include "../widgets/dbreathinglabel.h"
#include "dbus/dbuspower.h"

#include "moduleheader.h"
DWIDGET_USE_NAMESPACE

class PowerManagement: public QFrame
{
    Q_OBJECT
public:
    PowerManagement(QWidget *parent = 0);
    ~PowerManagement();

public slots:
    void reset();
    void setbatteryExist(bool onBat);
    void initConnection();
    void addBatterys(QList<BatteryItem> bats);
    void setOnBattery(bool onBats);
signals:
    void Reset();
private:
    QString headTitle;
    bool m_onBattery = false;
    bool m_batteryIsPresent;
    double m_batteryPercentage;
    // top header
    QVBoxLayout *topHeaderLayout;
    ModuleHeader *m_powerModuleHeader;
    QLabel *m_moduleNameLabel;
    QLabel *m_powerTitleLabel;


    DBreathingLabel* m_batteryPercentageLabel;
    QList<BatteryItem> m_batteryInfoList;

    QHBoxLayout *m_moduleTitleLayout;
    DTextButton *m_powerResetButton;
    DSeparatorHorizontal *m_firstHSeparator;
};
#endif // powermanager

