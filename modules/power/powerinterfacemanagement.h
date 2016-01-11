#ifndef POWERINTERFACEMANAGEMENT
#define POWERINTERFACEMANAGEMENT
#include <QObject>

#include "dbus/dbuspower.h"

class PowerInterfaceManagement:public QObject
{
    Q_OBJECT
public:
    PowerInterfaceManagement(QObject *parent=0);
    ~PowerInterfaceManagement();

    com::deepin::daemon::DBusPower* m_powerInterface;
signals:
    void PowerButtonActionChanged();
    void LidClosedActionChanged();
    void LockWhenActiveChanged();
    void LinePowerPlanChanged();
    void LinePowerIdleDelayChanged();
    void LinePowerSuspendDelayChanged();

    void BatteryPlanChanged();
    void BatteryIdleDelayChanged();
    void BatterySuspendDelayChanged();
    void BatteryPercentageChanged();
    void BatteryPercentageChanged(double percentage);
    void BatteryIsPresentChanged();

public slots:
    void initConnection();
    qint32 getPowerButtonAction();
    qint32 getLidCloseAction();
    void setPowerButtonAction(int index);
    void setLidCloseAction(int index);

    bool getLockWhenActive();
    void setLockWhenActive(bool isneedPassWd);

    qint32 getLinePowerPlan();
    void setLinePowerPlan(int index);

    qint32 getLinePowerIdleDelay();
    void setLinePowerIdleDelay(int index);
    qint32 getLinePowerSuspendDelay();
    void setLinePowerSuspendDelay(int index);

    qint32 getBatteryPlan();
    void setBatteryPlan(int index);

    qint32 getBatteryIdleDelay();
   void setBatteryIdleDelay(int index);
    qint32 getBatterySuspendDelay();
    void setBatterySuspendDelay(int index);
    void Reset();
    bool getBatteryIsPresent();
    bool getBatteryon();
    double getBatteryPresent();
    void batteryPresentUpdate();
    QString setPowerTooltipText(int index, QString powerType);
private:
    const int CLOSE_ACTION_SHUTDOWN = 2;
    const int CLOSE_ACTION_SUSPEND = 1;
    const int CLOSE_ACTION_INTERACTIVE = 4;
    const int CLOSE_ACTION_NOTHING = 5;

    int M_POWER_SUSPEND_DELAY = 0;
    int M_POWER_IDLE_DELAY = 0;
    int M_BATTERY_SUSPEND_DELAY = 0;
    int M_BATTERY_IDLE_DELAY = 0;
    void powerBatteryPlanInfo();
};
#endif // POWERINTERFACEMANAGEMENT

