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
#include "powerinterface.h"
#include "dynamiclabel.h"
#include "dbreathinglabel.h"

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
//    DSeparatorHorizontal* m_secondHSeparator;
    DHeaderLine* m_chooseNeedPasswdLine;
    DSwitchButton* m_chooseNeedPasswdButton;
    DSeparatorHorizontal* m_thirdHSeparator;
//  connection power
    QLabel* m_powerConnection;
    QVBoxLayout* powerConnectLayout;
    QStringList m_powerPerformaceString;
    DButtonGrid* m_linePowerButtonGrid;
    DHeaderLine* m_prePowerSettingHeaderLine;
    DBaseLine* m_powerSettingDBaseLine;
//    DBaseLine* m_powerSettingBaseLine;
    DBreathingLabel* m_powerConnectionBreathingLabel;
    DSeparatorHorizontal* m_forthHSeparator;
    DBaseExpand* m_powerSettingExpand;
    DExtendBoard* m_powerCustomExtendBoard;
    DynamicLabel* m_powerDynamicLabel;
// use battery
    QVBoxLayout* batteryUsedLayout;
    DButtonGrid* m_batteryButtonGrid;
    DBaseLine* m_batterySettingDBaseLine;
    DBreathingLabel* m_batteryBreathingLabel;
    DSeparatorHorizontal* m_sixthHSeparator;
    DBaseExpand* m_batterySettingExpand;
    DExtendBoard* m_batteryCustomExtendBoard;
    DynamicLabel* m_batteryDynamicLabel;
    /* get the setting in dbus connection */
    bool m_batteryIsPresent;
    bool m_onBattery;
    double m_batteryPercentage;

    qint32 m_batteryState;//read-only
//interface
     PowerInterface* m_powerInter;
public slots:
    inline void Reset(bool reset) {
        if (reset) { m_powerInter->Reset();}
    }
    void initUI();
    void initClockWhenActiveUI();
    void initPowerConnectionPanelUI();
    void initBatteryUsedUI();

    void initData();
    void handleTestTimer();

// update the UI after get the Initialize data
    void updatePowerManagermentUI();
    void updatePowerLockUI();

    void updatePressPowerButtonReactionUI();
    void updatePressLidPowerButtonReactionUI();
    void updateBatteryUsedControlUI();

    void initConnection();

    inline void updateBatteryPlanUI() {
        qint32 batteryPlan = m_powerInter->getBatteryPlan();
        set4ButtonGridChecked(batteryPlan, m_batteryButtonGrid);
        setPowerAndBatteryCheckAndExpand(batteryPlan, m_batteryCustomExtendBoard);
        updateBatteryIdleDelayUI();
        updateBatterySuspendDelayUI();
    }
    inline void updateBatteryIdleDelayUI() {
        qint32 batteryIdleDelay = m_powerInter->getBatteryIdleDelay()/60;
        set7ButtonGridChecked(batteryIdleDelay, m_batteryCustomExtendBoard->m_idleButtonGrid);
    }
    inline void updateBatterySuspendDelayUI() {
        qint32 batterySuspendDelay = m_powerInter->getBatterySuspendDelay()/60;
        set7ButtonGridChecked(batterySuspendDelay, m_batteryCustomExtendBoard->m_suspendButtonGrid);
    }

    inline void updateLinePowerPlanUI() {
        qint32 linePowerPlan = m_powerInter->getLinePowerPlan();
        qDebug() << "linePowerPlan:" << linePowerPlan;
        set4ButtonGridChecked(linePowerPlan, m_linePowerButtonGrid);
        setPowerAndBatteryCheckAndExpand(linePowerPlan, m_powerCustomExtendBoard);
        updateLinePowerIdleDelayUI();
        updateLinePowerSuspendDelayUI();
    }
    inline void updateLinePowerIdleDelayUI() {
        qint32 linePowerIdleDelay = m_powerInter->getLinePowerIdleDelay()/60;
        set7ButtonGridChecked(linePowerIdleDelay, m_powerCustomExtendBoard->m_idleButtonGrid);
    }
    inline void updateLinePowerSuspendDelayUI() {
        qint32 linePowerSuspendDelay = m_powerInter->getLinePowerSuspendDelay()/60;
        set7ButtonGridChecked(linePowerSuspendDelay, m_powerCustomExtendBoard->m_suspendButtonGrid);
    }

    void set4ButtonGridChecked(int idIndex, DButtonGrid* buttonGroup);
    void set7ButtonGridChecked(int idIndex, DButtonGrid* buttonGroup);
    inline void setPowerAndBatteryExpand(int index, DExtendBoard* expandBoard) {
        if (index == 0) {expandBoard->setExpand(true);}
        else { expandBoard->setExpand(false);}
    }

    inline void setPowerAndBatteryCheckAndExpand(int idIndex, DExtendBoard* expandBoard) {
        setPowerAndBatteryExpand(idIndex, expandBoard);
    }
    inline void  setPowerDynamicTooltip(int index) {
        QString powerTips = m_powerInter->setPowerTooltipText(index, "power");
        m_powerDynamicLabel->setText(powerTips);
    }

    inline void  showPowerTooltip(int index) {
        setPowerDynamicTooltip(index);
        QFont labelFont; QFontMetrics fm(labelFont);
        QString tips = m_powerInter->setPowerTooltipText(index, "power");
        if (fm.width(tips) > 240) {
            m_powerConnectionBreathingLabel->hideLabel();
        }
        m_powerDynamicLabel->delayShowLabel(300);
    }
    inline void  hidePowerTooltip(int index) {
        QFont labelFont; QFontMetrics fm(labelFont);
        QString tips = m_powerInter->setPowerTooltipText(index, "power");

        if (fm.width(tips) > 240) {
            m_powerConnectionBreathingLabel->show();
            m_powerConnectionBreathingLabel->showLabel();
        }

        m_powerDynamicLabel->hideLabel();
    }

    inline void  setBatteryDynamicTooltip(int index) {
        QString batteryTips = m_powerInter->setPowerTooltipText(index, "battery");
        m_batteryDynamicLabel->setText(batteryTips);
    }


    inline void  showBatteryTooltip(int index) {
        setBatteryDynamicTooltip(index);
        QFont labelFont; QFontMetrics fm(labelFont);
        QString tips = m_powerInter->setPowerTooltipText(index, "battery");

        if (fm.width(tips) > 240) {
            m_batteryBreathingLabel->hideLabel();
        }

        m_batteryDynamicLabel->delayShowLabel(300);
    }

    inline void  hideBatteryTooltip(int index) {
        QFont labelFont; QFontMetrics fm(labelFont);
        QString tips = m_powerInter->setPowerTooltipText(index, "battery");

        if (fm.width(tips) > 240) {
            m_batteryBreathingLabel->show();
            m_batteryBreathingLabel->showLabel();
        }
        m_batteryDynamicLabel->hideLabel();
    }
};
#endif
