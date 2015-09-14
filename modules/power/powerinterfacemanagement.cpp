#include <QObject>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
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
    if (actionButton == "ShutDown") {
        m_powerInterface->setPowerButtonAction(2);
    } else if (actionButton == "Suspend") {
        m_powerInterface->setPowerButtonAction(1);
    } else {
        m_powerInterface->setPowerButtonAction(4);
    }
}
void PowerInterfaceManagement::setLidCloseAction(QString actionButton) {
    if (actionButton == "ShutDown") {
        m_powerInterface->setLidClosedAction(2);
    } else if (actionButton == "Suspend") {
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
    } else if(buttonPerformace == "Power saver") {
        m_powerInterface->setLinePowerPlan(1);
    } else if(buttonPerformace == "High performance") {
        m_powerInterface->setLinePowerPlan(3);
    } else {
        m_powerInterface->setLinePowerPlan(0);
    }
}
qint32 PowerInterfaceManagement::getLinePowerIdleDelay() {
    return m_powerInterface->linePowerIdleDelay();
}
void PowerInterfaceManagement::setLinePowerIdleDelay(QString linePowerIdleDelay) {
    bool endWithHour = linePowerIdleDelay.endsWith("h");
    bool endWithMinute = linePowerIdleDelay.endsWith("m");

    if (endWithHour) {
        int time = linePowerIdleDelay.replace("h", "").toInt();
        m_powerInterface->setLinePowerIdleDelay(time*60*60);
    } else if (endWithMinute) {
        int time = linePowerIdleDelay.replace("m", "").toInt();
        m_powerInterface->setLinePowerIdleDelay(time*60);
    } else {
        m_powerInterface->setLinePowerIdleDelay(0);
    }

}
qint32 PowerInterfaceManagement::getLinePowerSuspendDelay() {
    return m_powerInterface->linePowerSuspendDelay();
}
void PowerInterfaceManagement::setLinePowerSuspendDelay(QString linePowerSuspendDelay) {
    bool endWithHour = linePowerSuspendDelay.endsWith("h");
    bool endWithMinute = linePowerSuspendDelay.endsWith("m");
    if (endWithHour) {
        int time = linePowerSuspendDelay.replace("h", "").toInt();
        m_powerInterface->setLinePowerSuspendDelay(time*60*60);
    } else if (endWithMinute) {
        int time = linePowerSuspendDelay.replace("m", "").toInt();
        m_powerInterface->setLinePowerSuspendDelay(time*60);
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
    } else if(buttonPerformance == "Power saver") {
         m_powerInterface->setBatteryPlan(1);
    } else if(buttonPerformance == "High performance") {
         m_powerInterface->setBatteryPlan(3);
    } else {
         m_powerInterface->setBatteryPlan(0);
    }
}
qint32 PowerInterfaceManagement::getBatteryIdleDelay() {
    return m_powerInterface->batteryIdleDelay();
}
void PowerInterfaceManagement::setBatteryIdleDelay(QString batteryIdleDelay) {
    bool endWithHour = batteryIdleDelay.endsWith("h");
    bool endWithMinute = batteryIdleDelay.endsWith("m");
    if (endWithHour) {
        int time = batteryIdleDelay.replace("h", "").toInt();
        m_powerInterface->setBatteryIdleDelay(time*60*60);
    } else if (endWithMinute) {
        int time = batteryIdleDelay.replace("m", "").toInt();
        m_powerInterface->setBatteryIdleDelay(time*60);
    } else {
        m_powerInterface->setBatteryIdleDelay(0);
    }
}
qint32 PowerInterfaceManagement::getBatterySuspendDelay() {
    return m_powerInterface->batterySuspendDelay();
}
void PowerInterfaceManagement::setBatterySuspendDelay(QString batterySuspendDelay) {
    bool endWithHour = batterySuspendDelay.endsWith("h");
    bool endWithMinute = batterySuspendDelay.endsWith("m");
    if (endWithHour) {
        int time = batterySuspendDelay.replace("h", "").toInt();
        m_powerInterface->setBatterySuspendDelay(time*60*60);
    } else if (endWithMinute) {
        int time = batterySuspendDelay.replace("m", "").toInt();
        m_powerInterface->setBatterySuspendDelay(time*60);
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
void PowerInterfaceManagement::batteryPresentUpdate() {
    emit this->BatteryPercentageChanged(m_powerInterface->batteryPercentage());
}
QString PowerInterfaceManagement::setPowerTooltipText(QString itemId, QString powerType) {
    QString argument_display, argument_suspend;

    if (itemId == "Balanced") {
        argument_display = QString("10 minutes");
        argument_suspend = QString("never");
    } else if (itemId == "Power saver") {
        argument_display = QString("15 minutes");
        argument_suspend = QString("5 minutes");
    } else if (itemId == "High performance") {
        argument_display = QString("15 minutes");
        argument_suspend = QString("never");
    } else {
        qint32 suspendTime, idleTime;
        if (powerType == "power") {
            suspendTime = m_powerInterface->linePowerSuspendDelay()/60;
            idleTime = m_powerInterface->linePowerIdleDelay()/60;
        } else {
            suspendTime = m_powerInterface->batterySuspendDelay()/60;
            idleTime = m_powerInterface->batteryIdleDelay()/60;
        }
        /////////////////idleTime
        if (idleTime==0) {
            argument_display = QString(tr("never"));
        } else if (idleTime/60!=0){
            argument_display = QString(tr("1hour"));
        } else {
            argument_display = QString(tr("%1minutes").arg(idleTime));
        }
        ///////////////suspendTime
        if (suspendTime==0) {
            argument_suspend = QString(tr("never"));
        } else if (suspendTime/60!=0){
            argument_suspend = QString(tr("1hour"));
        } else {
            argument_suspend = QString(tr("%1minutes").arg(suspendTime));
        }

    }

    QString tooltip_content = QString(tr("Turn off the display: %1 Suspend: %2 ").arg(argument_display).arg(argument_suspend));
    QFont labelFont;
    QFontMetrics fm(labelFont);
    int width=fm.width(tooltip_content);
    if (width>=250) {
        tooltip_content = QString(tr("Turn off the display: %1 \n Suspend: %2 ").arg(argument_display).arg(argument_suspend));
    }
    return tooltip_content;
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
   connect(m_powerInterface, SIGNAL(BatteryPercentageChanged()), SLOT(batteryPresentUpdate()));
   connect(m_powerInterface, SIGNAL(BatterySuspendDelayChanged()), SIGNAL(BatteryTooltipTextChanged()));
   connect(m_powerInterface, SIGNAL(BatteryIdleDelayChanged()), SIGNAL(BatteryTooltipTextChanged()));

}
