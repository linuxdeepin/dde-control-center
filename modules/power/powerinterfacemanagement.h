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
    void BatteryIsPresentChanged();

public slots:
    void initConnection();
    qint32 getPowerButtonAction();
    qint32 getLidCloseAction();
    void setPowerButtonAction(QString actionButton);
    void setLidCloseAction(QString actionButton);

    bool getLockWhenActive();
    void setLockWhenActive(bool isneedPassWd);

    qint32 getLinePowerPlan();
    void setLinePowerPlan(QString buttonPerformace);
    qint32 getLinePowerIdleDelay();
    void setLinePowerIdleDelay(QString linePowerIdleDelay);
    qint32 getLinePowerSuspendDelay();
    void setLinePowerSuspendDelay(QString linePowerSuspendDelay);

    qint32 getBatteryPlan();
    void setBatteryPlan(QString buttonPerformance);
    qint32 getBatteryIdleDelay();
    void setBatteryIdleDelay(QString batteryIdleDelay);
    qint32 getBatterySuspendDelay();
    void setBatterySuspendDelay(QString batterySuspendDelay);

    void Reset();
    bool getBatteryIsPresent();
    bool getBatteryon();
    double getBatteryPresent();
};
#endif // POWERINTERFACEMANAGEMENT

