#include "powerinterface.h"

PowerInterface::PowerInterface(QObject *parent)
    : QObject(parent),
      m_dbusPower(new DBusPower)
{
    getVirtualBatteryInfos();
    initConnection();
}

PowerInterface::~PowerInterface() {

}

DBusPower* PowerInterface::getDBusPowerIterface() {
    return m_dbusPower;
}

int PowerInterface::getScreenBlackDelay() {
    const int batBDelay =  m_dbusPower->batteryScreenBlackDelay();
    const int linePowerBDelay = m_dbusPower->linePowerScreenBlackDelay();
    const int tmpDelay = qMin(batBDelay, linePowerBDelay);
    return qMax(tmpDelay, 0);
}

void PowerInterface::setScreenBlackDelay(int time) {
    m_dbusPower->setBatteryScreenBlackDelay(time);
    m_dbusPower->setLinePowerScreenBlackDelay(time);

}

int PowerInterface::getSleepDelay() {
    const int batSDelay = m_dbusPower->batterySleepDelay();
    const int linePowerSDelay = m_dbusPower->linePowerSleepDelay();
    const int tmpDelay = qMin(batSDelay, linePowerSDelay);
    return  qMax(tmpDelay, 0);
}

void PowerInterface::setSleepDelay(int time) {
    m_dbusPower->setBatterySleepDelay(time);
    m_dbusPower->setLinePowerSleepDelay(time);
}

bool PowerInterface::getScreenBlackNeedPassWd() {
    return m_dbusPower->screenBlackLock();
}

void PowerInterface::setScreenBlackNeedPassWd(bool need) {
    m_dbusPower->setScreenBlackLock(need);
}

void PowerInterface::handleScreenBlackNeedPassWd() {
    if (m_dbusPower->screenBlackLock()) {
        emit screenBlackRequirePassWd(true);
    }  else {
        emit screenBlackRequirePassWd(false);
    }
}

bool PowerInterface::getSleepNeedPassWd() {
    return m_dbusPower->sleepLock();
}
void PowerInterface::setSleepNeedPassWd(bool need) {
    m_dbusPower->setSleepLock(need);
}

void PowerInterface::handleSleepNeedPassWd() {
    if (m_dbusPower->sleepLock()) {
        emit sleepRequiredPassWd(true);
    } else {
        emit sleepRequiredPassWd(false);
    }
}
void PowerInterface::setLidCloseNeedPassWd(bool need) {
    if (need) {
        qDebug() << "true";
        m_dbusPower->setLidClosedAction(LID_CLOSE_DEFAULTACTION);
    } else {
        qDebug() << "false";
        m_dbusPower->setLidClosedAction("");
    }
}

bool PowerInterface::getLidCloseNeedPassWd() {
    qDebug() << "m_dbusPower:" << m_dbusPower->lidClosedAction();
    if (!m_dbusPower->lidClosedAction().isEmpty()) {
        return true;
    } else {
        return false;
    }
}

void PowerInterface::handleLidCloseAction() {
    if (m_dbusPower->lidClosedAction().isEmpty()) {
        emit lidCloseRequirePassWd(false);
    } else {
        emit lidCloseRequirePassWd(true);
    }
}

//! getVirtualBatteryInfos will return all battery cap as one virtual battery.
/*!
  \return The virtaul battary item list
*/
QList<BatteryItem> PowerInterface::getVirtualBatteryInfos() {
    QList<BatteryItem> batterys;
    BatteryItem virtualBattery;
    BatteryInfoMap infoMap = m_dbusPower->batteryIsPresent();
    BatteryStateMap stateMap = m_dbusPower->batteryState();
    BatteryPercentageMap pertMap = m_dbusPower->batteryPercentage();

    if (stateMap.find(virtualBattery.batteryName) == stateMap.end()) {
        return batterys;
    }

    virtualBattery.batteryName = "Display";
    virtualBattery.batteryIsAccess = infoMap.value(virtualBattery.batteryName, false);
    virtualBattery.batteryPercentage = pertMap.value(virtualBattery.batteryName, 0);
    virtualBattery.batteryState = stateMap.value(virtualBattery.batteryName, BatteryStateUnkonwn);
    batterys.push_back(virtualBattery);

    return batterys;
}

void PowerInterface::handleBatteryPercentageChanged() {
    QList<BatteryItem> tmpItemList;
    tmpItemList = getVirtualBatteryInfos();
    qDebug() << "***handleBatteryPercentageChanged:" << tmpItemList.length();
    emit batteryPercentageChanged(tmpItemList);
}

void PowerInterface::initConnection() {
    connect(m_dbusPower, &DBusPower::ScreenBlackLockChanged, this, &PowerInterface::handleScreenBlackNeedPassWd);
    connect(m_dbusPower, &DBusPower::SleepLockChanged, this, &PowerInterface::handleSleepNeedPassWd);
    connect(m_dbusPower, &DBusPower::LidClosedActionChanged, this, &PowerInterface::handleLidCloseAction);
    connect(m_dbusPower, &DBusPower::BatteryPercentageChanged, this, &PowerInterface::handleBatteryPercentageChanged);
}
