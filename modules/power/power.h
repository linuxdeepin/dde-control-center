#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QtPlugin>
#include <QLabel>

#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dbaseline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dslider.h>
#include <libdui/dsegmentedcontrol.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dbaseexpand.h>

#include "interfaces.h"
#include "dbus/dbuspower.h"
#include "dextendboard.h"
#include "powermanagement.h"
#include "presspowerbuttonaction.h"
#include "powerinterfacemanagement.h"

DUI_USE_NAMESPACE
class QLabel;
class QFrame;
class Power: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "power.json")
    Q_INTERFACES(ModuleInterface)

public:
    Power();
    ~Power() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;


private:
    int m_contentHeight;
    int m_bgContentHeight;
    QLabel* m_label;
    QFrame* m_frame;
    com::deepin::daemon::DBusPower* m_powerInterface;
    PowerInterfaceManagement* m_powerInterfaceManagement;
//// top header
    PowerManagement* m_powerManagementFrame;

//  press power button action
    QStringList linePowerAction;
    PressPowerButtonAction* m_pressPowerButtonActionFrame;

// close the laptop
    QStringList closeLaptopAction;
    PressPowerButtonAction* m_closeLaptopActionFrame;
//   lockWhenActive
    QVBoxLayout* lockWhenActiveLayout;
    DSeparatorHorizontal* m_secondHSeparator;
    DHeaderLine* m_chooseNeedPasswdLine;
    DSwitchButton* m_chooseNeedPasswdButton;
    DSeparatorHorizontal* m_thirdHSeparator;
//  connection power
    QLabel* m_powerConnection;
    QVBoxLayout* powerConnectLayout;
    QStringList m_powerPerformaceString;
    DButtonGrid* m_powerPerformanceButtonGroup;
    DHeaderLine* m_prePowerSettingHeaderLine;
    DHeaderLine* m_powerSettingDHeaderLine;
    DHeaderLine* m_powerSettingBaseLine;
    DSeparatorHorizontal* m_forthHSeparator;
    DBaseExpand* m_powerSettingExpand;
    DExtendBoard* m_powerCustomExtendBoard;
// use battery
    QVBoxLayout* batteryUsedLayout;
    DButtonGrid* m_batteryButtonGrid;
    DHeaderLine* m_preBatterySettingHeaderLine;
    DHeaderLine* m_batterySettingDHeaderLine;
    DSeparatorHorizontal* m_sixthHSeparator;
    DBaseExpand* m_batterySettingExpand;
    DExtendBoard* m_batteryCustomExtendBoard;

    /* get the setting in dbus connection */
    bool m_batteryIsPresent;
    bool m_lockWhenActive;
    bool m_onBattery;
    double m_batteryPercentage;
    qint32 m_batteryIdleDelay;
    qint32 m_batteryPlan;
    qint32 m_batterySuspendDelay;
     qint32 m_batteryState;//read-only
    /* close the notebook */
    qint32 m_lidCloseAction;
    qint32 m_linePowerIdleDelay;
    qint32 m_linePowerSuspendDelay;
    qint32 m_powerButtonAction;

signals:

public slots:
    void Reset(bool reset);
    void initialUI();
    void initialCloseLapTopActionUI();
    void initialClockWhenActiveUI();
    void initialPowerConnectionPanelUI();
    void initialBatteryUsedUI();

    void initialData();

    void initialConnection();
    //powerType is setPowerButtonAction or setLidCloseAction

    void setLockWhenActive(bool isneedPassWd);

    void getBatteryReservedControl();
    void getLaptopCapCloseControl();
    void getBatteryUsedControl();

    void getBatteryPlan();
    void setBatteryPlan(QString buttonPerformance);
    void getBatteryIdleDelay();
    void setBatteryIdleDelay(QString batteryIdleDelay);
    void getBatterySuspendDelay();
    void setBatterySuspendDelay(QString batterySuspendDelay);

    void getLinePowerPlan();
    void setLinePowerPlan(QString buttonPerformace);
    void getLinePowerIdleDelay();
    void setLinePowerIdleDelay(QString linePowerIdleDelay);
    void getLinePowerSuspendDelay();
    void setLinePowerSuspendDelay(QString linePowerSuspendDelay);

};
#endif
