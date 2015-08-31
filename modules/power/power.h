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

    QFrame* m_frame;
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
    bool m_onBattery;
    double m_batteryPercentage;

    qint32 m_batteryState;//read-only

signals:

public slots:
    inline void Reset(bool reset) {
        if (reset) { m_powerInterfaceManagement->Reset();}
    }
    void initUI();
    void initClockWhenActiveUI();
    void initPowerConnectionPanelUI();
    void initBatteryUsedUI();

    void initData();

// update the UI after get the Initialize data
    void updateBatteryReservedControlUI();
    void updateLaptopCapCloseControlUI();
    void updateBatteryUsedControlUI();

    void initConnection();

    inline void updateBatteryPlanUI() {
        qint32 batteryPlan = m_powerInterfaceManagement->getBatteryPlan();
        setPowerAndBatteryCheckAndExpand(batteryPlan, m_batteryButtonGrid, m_batteryCustomExtendBoard);
    }
    inline void updateBatteryIdleDelayUI() {
        qint32 batteryIdleDelay = m_powerInterfaceManagement->getBatteryIdleDelay()/60;
        set7ButtonGridChecked(batteryIdleDelay, m_batteryCustomExtendBoard->m_poweroffButtonGrid);
    }
    inline void updateBatterySuspendDelayUI() {
        qint32 batterySuspendDelay = m_powerInterfaceManagement->getBatterySuspendDelay()/60;
        set7ButtonGridChecked(batterySuspendDelay, m_batteryCustomExtendBoard->m_standByButtonGrid);
    }
    inline void setUseBatteryExpand(QString buttonId) {
        setPowerAndBatteryExpand(buttonId, m_batteryCustomExtendBoard);
    }

    inline void updateLinePowerPlanUI() {
        qint32 linePowerPlan = m_powerInterfaceManagement->getLinePowerPlan();
        setPowerAndBatteryCheckAndExpand(linePowerPlan, m_powerPerformanceButtonGroup,
           m_powerCustomExtendBoard);
    }
    inline void updateLinePowerIdleDelayUI() {
        qint32 linePowerIdleDelay = m_powerInterfaceManagement->getLinePowerIdleDelay()/60;
        set7ButtonGridChecked(linePowerIdleDelay, m_powerCustomExtendBoard->m_poweroffButtonGrid);
    }
    inline void updateLinePowerSuspendDelayUI() {
        qint32 linePowerSuspendDelay = m_powerInterfaceManagement->getLinePowerSuspendDelay()/60;
        set7ButtonGridChecked(linePowerSuspendDelay, m_powerCustomExtendBoard->m_standByButtonGrid);
    }
    inline void setConnectPowerExpand(QString buttonId) {
        setPowerAndBatteryExpand(buttonId, m_powerCustomExtendBoard);
    }
    void set4ButtonGridChecked(int idIndex, DButtonGrid* buttonGroup);
    void set7ButtonGridChecked(int idIndex, DButtonGrid* buttonGroup);
    inline void setPowerAndBatteryExpand(QString buttonId, DExtendBoard* expandBoard) {
        if (buttonId == "Custom") {expandBoard->setExpand(true);}
        else { expandBoard->setExpand(false);}
    }
    inline void setPowerAndBatteryExpand(int idIndex, DExtendBoard* expandBoard) {
        if (idIndex==0) { expandBoard->setExpand(true);}
        else { expandBoard->setExpand(false);}
    }
    inline void setPowerAndBatteryCheckAndExpand(int idIndex, DButtonGrid* buttonGroup,DExtendBoard* expandBoard) {
        setPowerAndBatteryExpand(idIndex, expandBoard);
        set4ButtonGridChecked(idIndex, buttonGroup);
    }
};
#endif
