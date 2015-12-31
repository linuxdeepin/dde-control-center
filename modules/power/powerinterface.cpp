#include <QObject>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

#include "powerinterface.h"

PowerInterface::PowerInterface(QObject *parent)
    : QObject(parent)
{
    m_powerInterface = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                      "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
    initConnection();
}
PowerInterface::~PowerInterface() {

}
qint32 PowerInterface::getPowerButtonAction() {
    return m_powerInterface->powerButtonAction();
}
qint32 PowerInterface::getLidCloseAction() {
    return m_powerInterface->lidClosedAction();
}
void PowerInterface::setPowerButtonAction(int index) {
    if (index == 0) {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_SHUTDOWN);
    } else if (index == 1) {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_SUSPEND);
    } else {
        m_powerInterface->setPowerButtonAction(CLOSE_ACTION_INTERACTIVE);
    }
}
void PowerInterface::setLidCloseAction(int index) {
    if (index == 0) {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_SHUTDOWN);
    } else if (index == 1) {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_SUSPEND);
    } else {
        m_powerInterface->setLidClosedAction(CLOSE_ACTION_NOTHING);
    }
}
bool PowerInterface::getLockWhenActive() {
    return m_powerInterface->lockWhenActive();
}
void PowerInterface::setLockWhenActive(bool isneedPassWd) {
    m_powerInterface->setLockWhenActive(isneedPassWd);
}
qint32 PowerInterface::getLinePowerPlan() {
    return m_powerInterface->linePowerPlan();
}
void PowerInterface::setLinePowerPlan(int index) {
    qDebug() << "setLinePowerPlan:" << index;
    if (index == 0) {
        m_powerInterface->setLinePowerPlan(2);
    } else if(index == 1) {
        m_powerInterface->setLinePowerPlan(1);
    } else if(index == 2) {
        m_powerInterface->setLinePowerPlan(3);
    } else {
        m_powerInterface->setLinePowerPlan(0);
    }
}
qint32 PowerInterface::getLinePowerIdleDelay() {
    return m_powerInterface->linePowerIdleDelay();
}

void PowerInterface::setLinePowerIdleDelay(int index) {
    switch(index) {
    case 0: m_powerInterface->setLinePowerIdleDelay(60);    break;
    case 1: m_powerInterface->setLinePowerIdleDelay(5*60);  break;
    case 2: m_powerInterface->setLinePowerIdleDelay(10*60); break;
    case 3: m_powerInterface->setLinePowerIdleDelay(15*60); break;
    case 4: m_powerInterface->setLinePowerIdleDelay(30*60); break;
    case 5: m_powerInterface->setLinePowerIdleDelay(3600);  break;
    case 6: m_powerInterface->setLinePowerIdleDelay(0);     break;
    }

}

qint32 PowerInterface::getLinePowerSuspendDelay() {
    return m_powerInterface->linePowerSuspendDelay();
}

void PowerInterface::setLinePowerSuspendDelay(int index) {
    switch(index) {
    case 0: m_powerInterface->setLinePowerSuspendDelay(60);    break;
    case 1: m_powerInterface->setLinePowerSuspendDelay(5*60);  break;
    case 2: m_powerInterface->setLinePowerSuspendDelay(10*60); break;
    case 3: m_powerInterface->setLinePowerSuspendDelay(15*60); break;
    case 4: m_powerInterface->setLinePowerSuspendDelay(30*60); break;
    case 5: m_powerInterface->setLinePowerSuspendDelay(3600);  break;
    case 6: m_powerInterface->setLinePowerSuspendDelay(0);     break;
    }
}

qint32 PowerInterface::getBatteryPlan() {
    return m_powerInterface->batteryPlan();
}
void PowerInterface::setBatteryPlan(int index) {
    qDebug() << "setBatteryPaln:" << index;
    if (index == 0) {
         m_powerInterface->setBatteryPlan(2);
    } else if(index == 1) {
         m_powerInterface->setBatteryPlan(1);
    } else if(index == 2) {
         m_powerInterface->setBatteryPlan(3);
    } else {
         m_powerInterface->setBatteryPlan(0);
    }
}
qint32 PowerInterface::getBatteryIdleDelay() {
    return m_powerInterface->batteryIdleDelay();
}

void PowerInterface::setBatteryIdleDelay(int index) {
    switch(index) {
    case 0: m_powerInterface->setBatteryIdleDelay(60);    break;
    case 1: m_powerInterface->setBatteryIdleDelay(5*60);  break;
    case 2: m_powerInterface->setBatteryIdleDelay(10*60); break;
    case 3: m_powerInterface->setBatteryIdleDelay(15*60); break;
    case 4: m_powerInterface->setBatteryIdleDelay(30*60); break;
    case 5: m_powerInterface->setBatteryIdleDelay(3600);  break;
    case 6: m_powerInterface->setBatteryIdleDelay(0);     break;
    }
}
qint32 PowerInterface::getBatterySuspendDelay() {
    return m_powerInterface->batterySuspendDelay();
}
void PowerInterface::setBatterySuspendDelay(int index) {
    switch(index) {
    case 0: m_powerInterface->setBatterySuspendDelay(60);    break;
    case 1: m_powerInterface->setBatterySuspendDelay(5*60);  break;
    case 2: m_powerInterface->setBatterySuspendDelay(10*60); break;
    case 3: m_powerInterface->setBatterySuspendDelay(15*60); break;
    case 4: m_powerInterface->setBatterySuspendDelay(30*60); break;
    case 5: m_powerInterface->setBatterySuspendDelay(3600);  break;
    case 6: m_powerInterface->setBatterySuspendDelay(0);     break;
    }
}
void PowerInterface::Reset() {
    m_powerInterface->Reset();
}
bool PowerInterface::getBatteryIsPresent() {
    return m_powerInterface->batteryIsPresent();
}
bool PowerInterface::getBatteryon() {
    return m_powerInterface->onBattery();
}
double PowerInterface::getBatteryPresent() {
    return m_powerInterface->batteryPercentage();
}
void PowerInterface::batteryPresentUpdate() {
    emit this->BatteryPercentageChanged(m_powerInterface->batteryPercentage());
}
QString PowerInterface::setPowerTooltipText(int index, QString powerType) {
    QString argument_display, argument_suspend;
    if (index == 0) {
        argument_display = QString(tr("%1 minutes").arg("10"));
        argument_suspend = QString(tr("Never"));
    } else if (index == 1) {
        argument_display = QString(tr("%1 minutes")).arg("5");
        argument_suspend = QString(tr("%1 minutes")).arg("15");
    } else if (index == 2) {
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
void PowerInterface::powerBatteryPlanInfo() {
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

void PowerInterface::initConnection() {
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
