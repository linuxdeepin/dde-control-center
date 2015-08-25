#include <QObject>
#include "powerinterfacemanagement.h"

PowerInterfaceManagement::PowerInterfaceManagement(QObject *parent)
    : QObject(parent)
{
    m_powerInterface = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                      "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
}
PowerInterfaceManagement::~PowerInterfaceManagement() {

}
qint32 PowerInterfaceManagement::getPowerButtonAction() {
    return m_powerInterface->powerButtonAction();
}
qint32 PowerInterfaceManagement::getLidCloseAction() {
    return m_powerInterface->lidClosedAction();
}
void PowerInterfaceManagement::setPowerButtonAction(QString actionButton) {
    if (actionButton == "poweroff") {
        m_powerInterface->setPowerButtonAction(2);
    } else if (actionButton == "standby") {
        m_powerInterface->setPowerButtonAction(1);
    } else {
        m_powerInterface->setPowerButtonAction(4);
    }
}
void PowerInterfaceManagement::setLidCloseAction(QString actionButton) {
    if (actionButton == "poweroff") {
        m_powerInterface->setLidClosedAction(2);
    } else if (actionButton == "standby") {
        m_powerInterface->setLidClosedAction(1);
    } else {
        m_powerInterface->setLidClosedAction(4);
    }
}
bool PowerInterfaceManagement::getLockWhenActive() {
    return m_powerInterface->lockWhenActive();
}
void PowerInterfaceManagement::setLockWhenActive(bool isneedPassWd) {
    m_powerInterface->setLockWhenActive(isneedPassWd);
}
qint32 PowerInterfaceManagement::getLinePowerPlan() {
    return m_powerInterface->linePowerPlan();
}
void PowerInterfaceManagement::setLinePowerPlan(QString buttonPerformace) {

    if (buttonPerformace == "Balanced") {
        m_powerInterface->setLinePowerPlan(2);
    } else if(buttonPerformace == "PowerSaver") {
        m_powerInterface->setLinePowerPlan(1);
    } else if(buttonPerformace == "HighPerformance") {
        m_powerInterface->setLinePowerPlan(3);
    } else {
        m_powerInterface->setLinePowerPlan(0);
    }
}
qint32 PowerInterfaceManagement::getLinePowerIdleDelay() {
    return m_powerInterface->linePowerIdleDelay();
}
void PowerInterfaceManagement::setLinePowerIdleDelay(QString linePowerIdleDelay) {
    if (linePowerIdleDelay == "1m") {
        m_powerInterface->setLinePowerIdleDelay(1*60);
    } else if (linePowerIdleDelay == "5m") {
        m_powerInterface->setLinePowerIdleDelay(5*60);
    } else if (linePowerIdleDelay == "10m") {
        m_powerInterface->setLinePowerIdleDelay(10*60);
    } else if  (linePowerIdleDelay == "15m") {
        m_powerInterface->setLinePowerIdleDelay(15*60);
    } else if (linePowerIdleDelay == "30m") {
        m_powerInterface->setLinePowerIdleDelay(30*60);
    } else if (linePowerIdleDelay == "1h") {
        m_powerInterface->setLinePowerIdleDelay(3600);
    } else {
        m_powerInterface->setLinePowerIdleDelay(0);
    }
}
qint32 PowerInterfaceManagement::getLinePowerSuspendDelay() {
    return m_powerInterface->linePowerSuspendDelay();
}
void PowerInterfaceManagement::setLinePowerSuspendDelay(QString linePowerSuspendDelay) {
    if (linePowerSuspendDelay == "1m") {
        m_powerInterface->setLinePowerSuspendDelay(1*60);
    } else if (linePowerSuspendDelay == "5m") {
        m_powerInterface->setLinePowerSuspendDelay(5*60);
    } else if (linePowerSuspendDelay == "10m") {
        m_powerInterface->setLinePowerSuspendDelay(10*60);
    } else if  (linePowerSuspendDelay == "15m") {
        m_powerInterface->setLinePowerSuspendDelay(15*60);
    } else if (linePowerSuspendDelay == "30m") {
        m_powerInterface->setLinePowerSuspendDelay(30*60);
    } else if (linePowerSuspendDelay == "1h") {
        m_powerInterface->setLinePowerSuspendDelay(3600);
    } else {
        m_powerInterface->setLinePowerSuspendDelay(0);
    }
}
qint32 PowerInterfaceManagement::getBatteryPlan() {
    return m_powerInterface->batteryPlan();
}
void PowerInterfaceManagement::setBatteryPlan(QString buttonPerformance) {
    if (buttonPerformance == "Balanced") {
         m_powerInterface->setBatteryPlan(2);
    } else if(buttonPerformance == "PowerSaver") {
         m_powerInterface->setBatteryPlan(1);
    } else if(buttonPerformance == "HighPerformance") {
         m_powerInterface->setBatteryPlan(3);
    } else {
         m_powerInterface->setBatteryPlan(0);
    }
}
qint32 PowerInterfaceManagement::getBatteryIdleDelay() {
    return m_powerInterface->batteryIdleDelay();
}
void PowerInterfaceManagement::setBatteryIdleDelay(QString batteryIdleDelay) {
    if (batteryIdleDelay == "1m") {
        m_powerInterface->setBatteryIdleDelay(1*60);
    } else if (batteryIdleDelay == "5m") {
        m_powerInterface->setBatteryIdleDelay(5*60);
    } else if (batteryIdleDelay == "10m") {
        m_powerInterface->setBatteryIdleDelay(10*60);
    } else if (batteryIdleDelay == "15m") {
        m_powerInterface->setBatteryIdleDelay(15*60);
    } else if (batteryIdleDelay == "30m") {
        m_powerInterface->setBatteryIdleDelay(30*60);
    } else if (batteryIdleDelay == "1h") {
        m_powerInterface->setBatteryIdleDelay(60*60);
    } else {
        m_powerInterface->setBatteryIdleDelay(0);
    }
}
qint32 PowerInterfaceManagement::getBatterySuspendDelay() {
    return m_powerInterface->batterySuspendDelay();
}
void PowerInterfaceManagement::setBatterySuspendDelay(QString batterySuspendDelay) {
    if (batterySuspendDelay == "1m") {
        m_powerInterface->setBatterySuspendDelay(1*60);
    } else if (batterySuspendDelay == "5m") {
        m_powerInterface->setBatterySuspendDelay(5*60);
    } else if (batterySuspendDelay == "10m") {
        m_powerInterface->setBatterySuspendDelay(10*60);
    } else if (batterySuspendDelay == "15m") {
        m_powerInterface->setBatterySuspendDelay(15*60);
    } else if (batterySuspendDelay == "30m") {
        m_powerInterface->setBatterySuspendDelay(30*60);
    } else if (batterySuspendDelay == "1h") {
        m_powerInterface->setBatterySuspendDelay(60*60);
    } else {
        m_powerInterface->setBatterySuspendDelay(0);
    }
}
void PowerInterfaceManagement::Reset() {
    m_powerInterface->Reset();
}
bool PowerInterfaceManagement::getBatteryIsPresent() {
    return m_powerInterface->batteryIsPresent();
}
bool PowerInterfaceManagement::getBatteryon() {
    return m_powerInterface->onBattery();
}
double PowerInterfaceManagement::getBatteryPresent() {
    return m_powerInterface->batteryPercentage();
}
void PowerInterfaceManagement::initConnection() {
   connect(m_powerInterface, SIGNAL(LidClosedActionChanged()), SIGNAL(LidClosedActionChanged()));
   connect(m_powerInterface, SIGNAL(PowerButtonActionChanged()), SIGNAL(PowerButtonActionChanged()));

   connect(m_powerInterface, SIGNAL(LockWhenActiveChanged()), SIGNAL(LockWhenActiveChanged()));
   connect(m_powerInterface, SIGNAL(LinePowerPlanChanged()), SIGNAL(LinePowerPlanChanged()));

   connect(m_powerInterface, SIGNAL(LinePowerPlanChanged()), SIGNAL(LinePowerPlanChanged()));
   connect(m_powerInterface, SIGNAL(LinePowerIdleDelayChanged()), SIGNAL(LinePowerIdleDelayChanged()));
   connect(m_powerInterface, SIGNAL(LinePowerSuspendDelayChanged()), SIGNAL(LinePowerSuspendDelayChanged()));

   connect(m_powerInterface, SIGNAL(BatteryPlanChanged()), SIGNAL(BatteryPlanChanged()));
   connect(m_powerInterface, SIGNAL(BatteryIdleDelayChanged()), SIGNAL(BatteryIdleDelayChanged()));
   connect(m_powerInterface, SIGNAL(BatterySuspendDelayChanged()), SIGNAL(BatterySuspendDelayChanged()));

   connect(m_powerInterface, SIGNAL(BatteryIsPresentChanged()), SIGNAL(BatteryIsPresentChanged()));
   connect(m_powerInterface, SIGNAL(BatteryPercentageChanged()), SIGNAL(BatteryPercentageChanged()));
}
