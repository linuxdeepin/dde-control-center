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

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>

#include "moduleheader.h"
DUI_USE_NAMESPACE

class PowerManagement:public QFrame {
    Q_OBJECT
public:
    PowerManagement(QWidget *parent=0);
    ~PowerManagement();
public slots:
    void reset();
    void batteryReservedControl(bool batteryIsPresent);
    void setElectricQuantity(double electricQuantity);
    void initConnection();
signals:
    void Reset();
private:
    QString headTitle;
    bool m_batteryIsPresent;
    double m_batteryPercentage;
    // top header
    QVBoxLayout* topHeaderLayout;
    ModuleHeader* m_topHeaderLine;
    QLabel* m_powerManagerViewLabel;
    QLabel* m_powerManagerLabel;
    QLabel* m_shortSeparatorLine;
    QLabel* m_powerPercentageLabel;
    QHBoxLayout* m_powerManagerLayout;
    DTextButton* m_powerResetButton;
    DSeparatorHorizontal * m_firstHSeparator;
};
#endif // powermanager

