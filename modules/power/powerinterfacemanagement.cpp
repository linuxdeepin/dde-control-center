#include <QObject>
#include "powerinterfacemanagement.h"

PowerInterfaceManagement::PowerInterfaceManagement(QObject *parent)
    : QObject(parent)
{
    mn_powerInterface = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                      "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
}
PowerInterfaceManagement::~PowerInterfaceManagement() {

}
qint32 PowerInterfaceManagement::getPowerButtonAction() {
    return mn_powerInterface->powerButtonAction();
}
qint32 PowerInterfaceManagement::getLidCloseAction() {
    return mn_powerInterface->lidClosedAction();
}
void PowerInterfaceManagement::setPowerButtonAction(QString actionButton) {
    if (actionButton == "poweroff") {
        mn_powerInterface->setPowerButtonAction(2);
    } else if (actionButton == "standby") {
        mn_powerInterface->setPowerButtonAction(1);
    } else {
        mn_powerInterface->setPowerButtonAction(4);
    }
}
void PowerInterfaceManagement::setLidCloseAction(QString actionButton) {
    if (actionButton == "poweroff") {
        mn_powerInterface->setLidClosedAction(2);
    } else if (actionButton == "standby") {
        mn_powerInterface->setLidClosedAction(1);
    } else {
        mn_powerInterface->setLidClosedAction(4);
    }
}
void PowerInterfaceManagement::initialConnection() {
   connect(mn_powerInterface, SIGNAL(LidClosedActionChanged()), SIGNAL(LidClosedActionChanged()));
   connect(mn_powerInterface, SIGNAL(PowerButtonActionChanged()), SIGNAL(PowerButtonActionChanged()));
}
