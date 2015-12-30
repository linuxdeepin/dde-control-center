#include <QObject>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include "powerinterfacemanagement.h"

PowerInterfaceManagement::PowerInterfaceManagement(QObject *parent)
    : QObject(parent)
{
    m_powerInterface = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                      "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
    initConnection();
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
    if (actionButton == tr("Shutdown")) {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_SHUTDOWN);
    } else if (actionButton == qApp->translate("Power", "Suspend")) {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_SUSPEND);
    } else {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_INTERACTIVE);
    }
}
void PowerInterfaceManagement::setLidCloseAction(QString actionButton) {
    if (actionButton == tr("Shutdown")) {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_SHUTDOWN);
    } else if (actionButton == qApp->translate("Power", "Suspend")) {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_SUSPEND);
    } else {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_NOTHING);
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

    if (buttonPerformace == tr("Balanced")) {
        m_powerInterface->setLinePowerPlan(2);
    } else if(buttonPerformace == tr("Power saver")) {
        m_powerInterface->setLinePowerPlan(1);
    } else if(buttonPerformace == tr("High performance")) {
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
    if (buttonPerformance == tr("Balanced")) {
         m_powerInterface->setBatteryPlan(2);
    } else if(buttonPerformance == tr("Power saver")) {
         m_powerInterface->setBatteryPlan(1);
    } else if(buttonPerformance == tr("High performance")) {
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

    if (itemId == tr("Balanced")) {
        argument_display = QString(tr("%1 minutes").arg("10"));
        argument_suspend = QString(tr("Never"));
    } else if (itemId == tr("Power saver")) {
        argument_display = QString(tr("%1 minutes")).arg("5");
        argument_suspend = QString(tr("%1 minutes")).arg("15");
    } else if (itemId == tr("High performance")) {
        argument_display = QString(tr("%1 minutes")).arg("15");
        argument_suspend = QString(tr("Never"));
    } else {
        powerBatteryPlanInfo();
        qint32 suspendTime, idleTime;
        if (powerType == "power") {
            suspendTime = M_POWER_SUSPEND_DELAY/60;
            idleTime = M_POWER_IDLE_DELAY/60;
        } else {
            suspendTime = M_BATTERY_SUSPEND_DELAY/60;
            idleTime = M_BATTERY_IDLE_DELAY/60;
        }
        /////////////////idleTime
        if (idleTime==0) {
            argument_display = QString(tr("Never"));
        } else if (idleTime/60!=0){
            argument_display = QString(tr("1 hour"));
        } else {
            argument_display = QString(tr("%1 minutes").arg(idleTime));
        }
        ///////////////suspendTime
        if (suspendTime==0) {
            argument_suspend = QString(tr("Never"));
        } else if (suspendTime/60!=0){
            argument_suspend = QString(tr("1 hour"));
        } else {
            argument_suspend = QString(tr("%1 minutes").arg(suspendTime));
        }

    }

    QString tooltip_content = QString(tr("Turn off the display: %1 Suspend: %2 ").arg(argument_display).arg(argument_suspend));
    return tooltip_content;
}
void PowerInterfaceManagement::powerBatteryPlanInfo() {
    std::string planInfoString = m_powerInterface->planInfo().toStdString();
    QString m_planInfo = QString(QString::fromLocal8Bit(planInfoString.c_str()));

    QByteArray planInfo = m_planInfo.toLatin1();
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(planInfo, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        QJsonObject planInfoObject = parse_doucment.object();
        QJsonObject powerPlanInfoObject = planInfoObject.value("PowerLine").toObject();
        QJsonArray powerPlanInfoArray = powerPlanInfoObject.value("Custom").toArray();

        if (powerPlanInfoArray.count()==2) {
            M_POWER_IDLE_DELAY = powerPlanInfoArray.toVariantList()[0].toInt();
            M_POWER_SUSPEND_DELAY = powerPlanInfoArray.toVariantList()[1].toInt();
        }
        QJsonObject batteryPlanInfoObject = planInfoObject.value("Battery").toObject();
        QJsonArray batteryPlanInfoArray = batteryPlanInfoObject.value("Custom").toArray();

        if (batteryPlanInfoArray.count()==2) {
            M_BATTERY_IDLE_DELAY = batteryPlanInfoArray.toVariantList()[0].toInt();
            M_BATTERY_SUSPEND_DELAY = batteryPlanInfoArray.toVariantList()[1].toInt();
        }
    }
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

}
