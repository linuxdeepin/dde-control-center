/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QDebug>

#include <dbuttongrid.h>

#include "power.h"
#include "constants.h"

QFrame *PowerModule::getContent()
{
    qDebug() << "new Power begin";
    if (NULL == m_power) {
        m_power = new Power(this);
    }
    qDebug() << "new Power end";
    return m_power->getContent();
}

Power::Power(QObject *parent)
    : QObject(parent),
      m_frame(new QFrame),
      m_powerInterface(new PowerInterface),
      m_powerTitleManagement(new PowerManagement)

{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Power Management"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    initUI();
    updateUI();
    initConnect();
}

Power::~Power()
{
    qDebug() << "~Power()";
    m_frame->deleteLater();
}

void Power::initUI()
{
    m_screenBlackTimeWidget = new ChooseDelayTimeWidget(tr("Display will suspend after"));
    m_sleepTimeWidget = new ChooseDelayTimeWidget(tr("Computer will suspend after"));
    dsHorizontalFirst = new DSeparatorHorizontal;
    m_screenBlackSwitchWidget = new PowerSwitchWidget(tr("Password required to wake the display"));
    dsHorizontalSecond = new DSeparatorHorizontal;
    m_sleepSwitchWidget = new PowerSwitchWidget(tr("Password required to wake the computer"));
    dsHorizontalThird = new DSeparatorHorizontal;
    m_lidCloseSwitchWidget = new PowerSwitchWidget(tr("The notebook will suspend when close the lid"));
    dsHorizontalFourth = new DSeparatorHorizontal;

    QVBoxLayout *m_Layout = new QVBoxLayout(m_frame);
    m_Layout->setMargin(0);
    m_Layout->setSpacing(0);
    m_Layout->addWidget(m_powerTitleManagement);
    m_Layout->addWidget(m_screenBlackTimeWidget);
    m_Layout->addSpacing(4);
    m_Layout->addWidget(m_sleepTimeWidget);
    m_Layout->addWidget(dsHorizontalFirst);
    m_Layout->addWidget(m_screenBlackSwitchWidget);
    m_Layout->addWidget(dsHorizontalSecond);
    m_Layout->addWidget(m_sleepSwitchWidget);
    m_Layout->addWidget(dsHorizontalThird);
    m_Layout->addWidget(m_lidCloseSwitchWidget);
    m_Layout->addWidget(dsHorizontalFourth);
    m_Layout->addStretch();
    m_frame->setLayout(m_Layout);

}

void Power::updateUI()
{
    //for testint battery
//    BatteryItem a= {"BAT0", true, 99, 1};
//    BatteryItem b= {"BAT1", true, 100, 4};
//    BatteryItem c= {"BAT2", true, 100, 4};
//    QList<BatteryItem> batterys;
//    batterys.append(a);
//    batterys.append(b);
//    batterys.append(c);
//    m_powerTitleManagement->addBatterys(batterys);
    qDebug() << "begin to update ui";
    handleBatteryStateChanged();
    handleScreenBlackDelayChanged();
    handleSleepDelayChanged();
    handleScreenBlackLockChanged();
    handleSleepLockChanged();
    handleLidActionChanged();
}

void Power::handleScreenBlackDelayChanged()
{
    qDebug() << "handleScreenBlackDelayChanged!";
    int screenBlackTime = m_powerInterface->getScreenBlackDelay();
    qDebug() << "screenBlackTime:" << screenBlackTime;
    m_screenBlackTimeWidget->setCurrentTime(screenBlackTime);
}

void Power::handleSleepDelayChanged()
{
    qDebug() << "handleSleepDelayChanged!";
    int sleepTime = m_powerInterface->getSleepDelay();
    m_sleepTimeWidget->setCurrentTime(sleepTime);
}

void Power::handleScreenBlackLockChanged()
{
    qDebug() << "handleScreenBlackLockChanged!";
    bool screenBlackNeed = m_powerInterface->getScreenBlackNeedPassWd();
    qDebug() << "****updateUI:" << screenBlackNeed;
    m_screenBlackSwitchWidget->setChecked(screenBlackNeed);
}

void Power::handleSleepLockChanged()
{
    bool sleepNeed = m_powerInterface->getSleepNeedPassWd();
    m_sleepSwitchWidget->setChecked(sleepNeed);
}

void Power::handleBatteryStateChanged()
{
    qDebug() << "handleBatteryStateChanged:" << m_powerInterface->getDBusPowerIterface()->onBattery();
    m_powerTitleManagement->setOnBattery(m_powerInterface->getDBusPowerIterface()->onBattery());
    m_powerTitleManagement->addBatterys(m_powerInterface->getBatteryInfos());
}

void Power::handleLidActionChanged()
{
    if (m_powerInterface->getDBusPowerIterface()->lidIsPresent()) {
        bool lidCloseNeed = m_powerInterface->getLidCloseNeedPassWd();
        m_lidCloseSwitchWidget->setChecked(lidCloseNeed);
    } else {
        dsHorizontalFourth->hide();
        m_lidCloseSwitchWidget->hide();
    }
}

void Power::initConnect()
{
    connect(m_screenBlackTimeWidget, &ChooseDelayTimeWidget::currentTimeChanged, m_powerInterface, &PowerInterface::setScreenBlackDelay);
    connect(m_sleepTimeWidget, &ChooseDelayTimeWidget::currentTimeChanged, m_powerInterface, &PowerInterface::setSleepDelay);
    connect(m_screenBlackSwitchWidget, &PowerSwitchWidget::checkedChanged, m_powerInterface, &PowerInterface::setScreenBlackNeedPassWd);
    connect(m_sleepSwitchWidget, &PowerSwitchWidget::checkedChanged, m_powerInterface, &PowerInterface::setSleepNeedPassWd);
    connect(m_lidCloseSwitchWidget, &PowerSwitchWidget::checkedChanged, m_powerInterface, &PowerInterface::setLidCloseNeedPassWd);
    connect(m_powerInterface, &PowerInterface::batteryPercentageChanged, m_powerTitleManagement, &PowerManagement::addBatterys);
    connect(m_powerTitleManagement, &PowerManagement::Reset, m_powerInterface->getDBusPowerIterface(), &DBusPower::Reset);

    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::LidIsPresentChanged, this, &Power::handleLidActionChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::LidClosedActionChanged, this, &Power::handleLidActionChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::BatteryPercentageChanged, this, &Power::handleBatteryStateChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::OnBatteryChanged, this, &Power::handleBatteryStateChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::BatteryIsPresentChanged, this, &Power::handleBatteryStateChanged);
//    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::BatteryStateChanged, this, &Power::handleBatteryStateChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::LinePowerScreenBlackDelayChanged, this, &Power::handleScreenBlackDelayChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::BatteryScreenBlackDelayChanged, this, &Power::handleScreenBlackDelayChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::LinePowerSleepDelayChanged, this, &Power::handleSleepDelayChanged);

    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::ScreenBlackLockChanged, this, &Power::handleScreenBlackLockChanged);
    connect(m_powerInterface->getDBusPowerIterface(), &DBusPower::SleepLockChanged, this, &Power::handleSleepLockChanged);
}

QFrame *Power::getContent()
{
    return m_frame;
}
