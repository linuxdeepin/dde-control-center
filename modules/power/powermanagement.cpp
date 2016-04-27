/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QtWidgets/QFrame>
#include <QtGui/QFont>

#include "constants.h"
#include "powermanagement.h"

PowerManagement::PowerManagement(QWidget *parent)
    : QFrame(parent),
      m_firstHSeparator(new DSeparatorHorizontal)
{
    m_batteryIsPresent = false;
    m_batteryPercentage = 0;

    m_powerTitleLabel = new QLabel(this);
    m_powerTitleLabel->setFixedWidth(300);

    QString powerName(tr("Power Management"));
    m_moduleNameLabel = new QLabel(this);
    m_moduleNameLabel->setText(QString("%1").arg(powerName));
    QFont myFont;
    QFontMetrics fontMetrics(myFont);
    m_moduleNameLabel->setFixedWidth(fontMetrics.width(powerName)+19);

    m_batteryPercentageLabel = new DBreathingLabel("100%", this);
    m_batteryPercentageLabel->setColor(DCC::TextNormalColor);
    m_moduleTitleLayout = new QHBoxLayout;
    m_moduleTitleLayout->setMargin(0);
    m_moduleTitleLayout->setSpacing(0);
    m_moduleTitleLayout->addWidget(m_moduleNameLabel);
    m_moduleTitleLayout->addSpacing(10);
    m_moduleTitleLayout->addStretch(1);
    m_powerTitleLabel->setLayout(m_moduleTitleLayout);

    m_powerTitleLabel->sizeHint();
    m_batteryPercentageLabel->move(fontMetrics.width(powerName)+38, 10);

    m_powerModuleHeader = new ModuleHeader(m_powerTitleLabel);

    topHeaderLayout = new QVBoxLayout;
    topHeaderLayout->setMargin(0);
    topHeaderLayout->setSpacing(0);
    topHeaderLayout->addWidget(m_powerModuleHeader);
    topHeaderLayout->addWidget(m_firstHSeparator);
    setLayout(topHeaderLayout);

    initConnection();

}
PowerManagement::~PowerManagement() {
}

void PowerManagement::initConnection() {
    connect(m_powerModuleHeader, &ModuleHeader::resetButtonClicked, this, &PowerManagement::reset);
}
void PowerManagement::reset() {
    qDebug() << "power reset";
    emit Reset();
}

void PowerManagement::addBatterys(QList<BatteryItem> bats) {
    qDebug() << "bats" <<bats.length();
    m_batteryPercentage = 0;
    m_batteryInfoList.clear();
    for (int i=0; i < bats.length();i++) {
        m_batteryInfoList.append(bats[i]);
        m_batteryPercentage += m_batteryInfoList[i].batteryPercentage;

        if (i == bats.length() - 1) {
            m_batteryPercentage = m_batteryPercentage / bats.length();
            m_batteryPercentageLabel->setText(QString("%1%").arg(m_batteryPercentage));
            m_batteryPercentageLabel->setStyleSheet("font-size: 10pt;");
        }
    }


    if (!m_onBattery && m_batteryPercentage!=100) {
        m_batteryPercentageLabel->setCycle(1.5*1000);
    } else {
        m_batteryPercentageLabel->setStopCycle(true);
        m_batteryPercentageLabel->showLabel();
    }


    if (bats.length()!=0) {
        setbatteryExist(bats[0].batteryIsAccess);
    } else {
        setbatteryExist(false);
    }
}

void PowerManagement::setOnBattery(bool onBats) {
    m_onBattery = onBats;
}

void PowerManagement::setbatteryExist(bool batteryIsPresent) {
    m_batteryIsPresent = batteryIsPresent;

    if (!m_batteryIsPresent) {
        m_batteryPercentageLabel->hide();
    } else {
        m_batteryPercentageLabel->show();
    }

}

