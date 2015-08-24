#include <QLabel>
#include <QDebug>

#include "power.h"
#include "constants.h"

Power::Power()
{
    m_label = new QLabel();
    m_frame = new QFrame;
    m_contentHeight = 30;
    m_bgContentHeight = 60;
    m_label->setStyleSheet(QString("QLabel{color: %1;font-size:12px;}").arg(DCC::TextNormalColor.name()));

    m_powerInterface = new com::deepin::daemon::DBusPower("com.deepin.daemon.Power",
                                                      "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);

    m_powerInterfaceManagement = new PowerInterfaceManagement;

    initialUI();
    QVBoxLayout * layout = new QVBoxLayout(m_frame);
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_powerManagementFrame);
    layout->addWidget(m_pressPowerButtonActionFrame);
    layout->addWidget(m_closeLaptopActionFrame);

    layout->addLayout(lockWhenActiveLayout);  
    layout->addLayout(powerConnectLayout);
    layout->addLayout(batteryUsedLayout);
    layout->addStretch();
    m_frame->setLayout(layout);

    initialData();
    initialConnection();
}

Power::~Power()
{
    qDebug() << "~Power()";
    m_frame->deleteLater();

}

void Power::getBatteryUsedControl() {
    if (!m_batteryIsPresent) {
         m_batterySettingDHeaderLine->hide();
         m_batterySettingExpand->hide();
         m_batteryCustomExtendBoard->hide();
    } else {
        m_batterySettingDHeaderLine->show();

        m_batterySettingExpand->show();
        m_batteryCustomExtendBoard->show();
    }
}

void Power::Reset(bool reset) {
    if (reset) {
        m_powerInterface->Reset();
    }
}

void Power::initialClockWhenActiveUI() {
    m_secondHSeparator = new DSeparatorHorizontal(m_label);
    m_chooseNeedPasswdLine = new DHeaderLine(m_label);
    m_chooseNeedPasswdLine->setTitle("PasswdNeedForWakeUp");
    m_chooseNeedPasswdButton = new DSwitchButton;
    m_chooseNeedPasswdLine->setContent(m_chooseNeedPasswdButton);
    m_chooseNeedPasswdLine->setFixedHeight(m_contentHeight);
    m_thirdHSeparator = new DSeparatorHorizontal(m_label);

    lockWhenActiveLayout = new QVBoxLayout;
    lockWhenActiveLayout->setMargin(0);
    lockWhenActiveLayout->setSpacing(0);
    lockWhenActiveLayout->addWidget(m_secondHSeparator);
    lockWhenActiveLayout->addWidget(m_chooseNeedPasswdLine);
    lockWhenActiveLayout->addWidget(m_thirdHSeparator);
}
void Power::initialPowerConnectionPanelUI() {
    m_prePowerSettingHeaderLine = new DHeaderLine;
    m_powerSettingDHeaderLine = new DHeaderLine;
    m_powerSettingDHeaderLine->setTitle(tr("Connect Power"));

    m_powerSettingExpand = new DBaseExpand;
    m_powerSettingExpand->setFixedHeight(m_bgContentHeight);
    m_powerSettingExpand->setHeader(m_powerSettingDHeaderLine);
    m_powerSettingExpand->setHeaderHeight(30);


    m_powerPerformanceButtonGroup = new DButtonGrid(2, 2);

    m_powerPerformaceString << "Balanced" << "PowerSaver" << "HighPerformance" << "Custom";
    m_powerPerformanceButtonGroup->addButtons(m_powerPerformaceString);
    m_powerPerformanceButtonGroup->setItemSize(150, 30);
    m_powerPerformanceButtonGroup->checkButtonByIndex(2);
    m_powerSettingExpand->setContent(m_powerPerformanceButtonGroup);
    m_powerSettingExpand->setExpand(true);
/////////////////////////////////////////////////////-- power connection custom setting expand
    m_powerCustomExtendBoard = new DExtendBoard;
    m_powerCustomExtendBoard->setExpand(true);

    powerConnectLayout = new QVBoxLayout;
    powerConnectLayout->setMargin(0);
    powerConnectLayout->setSpacing(0);
    powerConnectLayout->addWidget(m_prePowerSettingHeaderLine);

    powerConnectLayout->addWidget(m_powerSettingDHeaderLine);

    powerConnectLayout->addWidget(m_powerSettingExpand);
    powerConnectLayout->addWidget(m_powerCustomExtendBoard);
}
void Power::initialBatteryUsedUI() {
    /////////////////////////////////////////////////////////--use battery setting panel
    m_preBatterySettingHeaderLine = new DHeaderLine;
    m_batterySettingDHeaderLine = new DHeaderLine;
    m_batterySettingDHeaderLine->setTitle(tr("UseBattery"));


    m_batterySettingExpand = new DBaseExpand;

    m_batterySettingExpand->setFixedHeight(m_bgContentHeight);
    m_batterySettingExpand->setHeader(m_batterySettingDHeaderLine);
    m_batterySettingExpand->setHeaderHeight(m_contentHeight);
    m_batteryButtonGrid = new DButtonGrid(2, 2);
    m_powerPerformaceString << "Balanced" << "PowerSaver" << "HighPerformance" << "Custom";
    m_batteryButtonGrid->addButtons(m_powerPerformaceString);
    m_batteryButtonGrid->setItemSize(150, 30);
    m_batteryButtonGrid->checkButtonByIndex(3);

    m_batterySettingExpand->setContent(m_batteryButtonGrid);
    m_batterySettingExpand->setExpand(true);
    m_batteryCustomExtendBoard = new DExtendBoard;
    m_batteryCustomExtendBoard->setExpand(true);


    batteryUsedLayout = new QVBoxLayout;
    batteryUsedLayout->setMargin(0);
    batteryUsedLayout->setSpacing(0);
    batteryUsedLayout->addWidget(m_preBatterySettingHeaderLine);
    batteryUsedLayout->addWidget(m_batterySettingDHeaderLine);
    batteryUsedLayout->addWidget(m_batterySettingExpand);
    batteryUsedLayout->addWidget(m_batteryCustomExtendBoard);
}
void Power::initialUI() {

    m_powerManagementFrame = new PowerManagement;

    linePowerAction  << "poweroff" << "standby" << "ask";
    m_pressPowerButtonActionFrame = new PressPowerButtonAction(linePowerAction);
    m_pressPowerButtonActionFrame->setTitle(tr("PressPowerButton"));
    m_pressPowerButtonActionFrame->setFixedHeight(m_bgContentHeight);

    closeLaptopAction << "poweroff" << "standby" << "no";
    m_closeLaptopActionFrame = new PressPowerButtonAction(closeLaptopAction);
    m_closeLaptopActionFrame->setTitle(tr("CloseLaptop"));
    m_closeLaptopActionFrame->setFixedHeight(m_bgContentHeight);

    initialClockWhenActiveUI();
    initialPowerConnectionPanelUI();
    initialBatteryUsedUI();
}
void Power::initialData() {
    ////////////////////////////////////////////////////--get battery setting
    m_batteryIsPresent = m_powerInterface->batteryIsPresent();
    m_onBattery = m_powerInterface->onBattery();
    m_batteryPercentage = m_powerInterface->batteryPercentage();

    ////////////////////////////////////////////////////-- lock when Active
    m_powerInterface->lockWhenActive();
    ////////////////////////////////////////////////////--getLinePowerPlan
    getLinePowerPlan();
    /////////////////////////////////////////////////////-- getLinePowerIdleDelay&SuspendDelay

    getLinePowerIdleDelay();
    getLinePowerSuspendDelay();
    getBatteryPlan();
    getBatteryIdleDelay();
    getBatterySuspendDelay();

    ////////////////////////////////////////////////////--setting the UI according to the data

    m_powerManagementFrame->setElectricQuantity(m_batteryPercentage);
    m_powerManagementFrame->batteryReservedControl();
    ////////////////////////////////////////////////////--get press power button action panel

    qint32 buttonId = m_powerInterfaceManagement->getPowerButtonAction();
    m_pressPowerButtonActionFrame->setPowerButtonAction(buttonId);

    qint32 buttonIndex = m_powerInterfaceManagement->getLidCloseAction();
    m_closeLaptopActionFrame->setPowerButtonAction(buttonIndex);
    m_chooseNeedPasswdButton->setChecked(m_powerInterface->lockWhenActive());
//    m_batteryIsPresent = true; for testing
    if (m_batteryIsPresent) {
        m_closeLaptopActionFrame->show();
    } else {
        m_closeLaptopActionFrame->hide();
    }

    getBatteryUsedControl();

}

void Power::setLockWhenActive(bool isneedPassWd) {
    m_powerInterface->setLockWhenActive(isneedPassWd);
}
void Power::getLinePowerPlan() {
    qint32 linePowerPlan = m_powerInterface->linePowerPlan();

    switch(linePowerPlan) {
        case 0: {
           m_powerPerformanceButtonGroup->checkButtonByIndex(3);
           m_powerCustomExtendBoard->setExpand(true);
           break;
        }
        case 1: {
            m_powerPerformanceButtonGroup->checkButtonByIndex(1);
            m_powerCustomExtendBoard->setExpand(false);
            break;
        }
        case 2: {
            m_powerPerformanceButtonGroup->checkButtonByIndex(0);
            m_powerCustomExtendBoard->setExpand(false);
            break;
        }
        case 3: {
            m_powerPerformanceButtonGroup->checkButtonByIndex(2);
            m_powerCustomExtendBoard->setExpand(false);
        }
    }
}
void Power::setLinePowerPlan(QString buttonPerformace) {
    if (buttonPerformace == "Balanced") {
        m_powerInterface->setLinePowerPlan(2);
    } else if(buttonPerformace == "PowerSaver") {
        m_powerInterface->setLinePowerPlan(1);
    } else if(buttonPerformace == "HighPerformance") {
        m_powerInterface->setLinePowerPlan(3);
    } else {
        m_powerInterface->setLinePowerPlan(0);
    }
    qDebug() << "buttonPerformace:" << buttonPerformace;
}


void Power::getLinePowerIdleDelay() {
    qint32 linePowerIdleDelay = m_powerInterface->linePowerIdleDelay()/60;
    qDebug() << "linePowerIdleDelay" << linePowerIdleDelay;
    switch (linePowerIdleDelay) {
        case 1:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(0);break;}
        case 5:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(1);break;}
        case 10:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(2);break;}
        case 15:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(3);break;}
        case 30:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(4);break;}
        case 60:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(5);break;}
        default:{ m_powerCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(6);}
    }
}
void Power::setLinePowerIdleDelay(QString linePowerIdleDelay) {
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
void Power::getLinePowerSuspendDelay() {
    qint32 linePowerSuspendDelay = m_powerInterface->linePowerSuspendDelay()/60;

    switch (linePowerSuspendDelay) {
        case 1:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(0);break;}
        case 5:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(1);break;}
        case 10:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(2);break;}
        case 15:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(3);break;}
        case 30:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(4);break;}
        case 60:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(5);break;}
        default:{ m_powerCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(6);}
    }
}
void Power::setLinePowerSuspendDelay(QString linePowerSuspendDelay) {

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
void Power::getBatteryPlan() {
    qint32 batteryPlan = m_powerInterface->batteryPlan();
    switch(batteryPlan) {
        case 0: {
           m_batteryButtonGrid->checkButtonByIndex(3);
           m_batteryCustomExtendBoard->setExpand(true);
           break;
        }
        case 1: {
            m_batteryButtonGrid->checkButtonByIndex(1);
            m_batteryCustomExtendBoard->setExpand(false);
            break;
        }
        case 2: {
            m_batteryButtonGrid->checkButtonByIndex(0);
            m_batteryCustomExtendBoard->setExpand(false);
            break;
        }
        case 3: {
            m_batteryButtonGrid->checkButtonByIndex(2);
            m_batteryCustomExtendBoard->setExpand(false);
        }
    }
}
void Power::setBatteryPlan(QString buttonPerformance) {
    if (buttonPerformance == "Balanced") {
        m_powerInterface->setBatteryPlan(2);
        m_batteryCustomExtendBoard->setExpand(false);
    } else if(buttonPerformance == "PowerSaver") {
        m_powerInterface->setBatteryPlan(1);
        m_batteryCustomExtendBoard->setExpand(false);
    } else if(buttonPerformance == "HighPerformance") {
        m_powerInterface->setBatteryPlan(3);
        m_batteryCustomExtendBoard->setExpand(false);
    } else {
        m_powerInterface->setBatteryPlan(0);
        if (m_batteryCustomExtendBoard->height() == 2) {
            m_batteryCustomExtendBoard->setFixedHeight(200);
        }
        m_batteryCustomExtendBoard->setExpand(true);
    }
}
void Power::getBatteryIdleDelay() {
    qint32 batteryIdleDelay = m_powerInterface->batteryIdleDelay()/60;
    switch (batteryIdleDelay) {
        case 1: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(0);break;}
        case 5: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(1);break;}
        case 10: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(2);break;}
        case 15: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(3);break;}
        case 30: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(4);break;}
        case 60: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(5);break;}
        default: { m_batteryCustomExtendBoard->m_poweroffButtonGrid->checkButtonByIndex(6);}
    }
}
void Power::setBatteryIdleDelay(QString batteryIdleDelay) {
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
void Power::getBatterySuspendDelay() {
    qint32 batterySuspendDelay = m_powerInterface->batterySuspendDelay()/60;

    switch (batterySuspendDelay) {
        case 1: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(0);break;}
        case 5: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(1);break;}
        case 10: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(2);break;}
        case 15: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(3);break;}
        case 30: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(4);break;}
        case 60: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(5);break;}
        default: { m_batteryCustomExtendBoard->m_standByButtonGrid->checkButtonByIndex(6);}
    }
}
void Power::setBatterySuspendDelay(QString batterySuspendDelay) {
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
void Power::initialConnection() {

    connect(m_powerInterface, SIGNAL(BatteryPercentageChanged()),
            this, SLOT(initialData()));
    connect(m_powerInterface, SIGNAL(BatteryIsPresentChanged()),
            this, SLOT(initialData()));

    connect(m_powerManagementFrame, SIGNAL(Reset()),
            m_powerInterface, SLOT(Reset()));
    connect(m_powerInterface, SIGNAL(LockWhenActiveChanged()),
            this, SLOT(initialData()));
    connect(m_chooseNeedPasswdButton, SIGNAL(checkedChanged(bool)),
            SLOT(setLockWhenActive(bool)));

    connect(m_powerInterfaceManagement, SIGNAL(PowerButtonActionChanged()),
             SLOT(initialData()));

    connect(m_pressPowerButtonActionFrame, SIGNAL(powerButtonAction(QString)),
            m_powerInterfaceManagement, SLOT(setPowerButtonAction(QString)));

    connect(m_powerInterfaceManagement, SIGNAL(LidClosedActionChanged()),
              SLOT(initialData()));

    connect(m_closeLaptopActionFrame, SIGNAL(powerButtonAction(QString)),
            m_powerInterfaceManagement, SLOT(setLidCloseAction(QString)));

    connect(m_powerInterface, SIGNAL(LinePowerPlanChanged()),
            this, SLOT(getLinePowerPlan()));
    connect(m_powerPerformanceButtonGroup, SIGNAL(buttonChecked(QString)),
            SLOT(setLinePowerPlan(QString)));

    connect(m_powerInterface, SIGNAL(LinePowerIdleDelayChanged()),
            this, SLOT(getLinePowerIdleDelay()));
    connect(m_powerCustomExtendBoard->m_poweroffButtonGrid,
            SIGNAL(buttonChecked(QString)),
            this, SLOT(setLinePowerIdleDelay(QString)));

    connect(m_powerInterface, SIGNAL(LinePowerSuspendDelayChanged()),
            this, SLOT(getLinePowerSuspendDelay()));
    connect(m_powerCustomExtendBoard->m_standByButtonGrid,
            SIGNAL(buttonChecked(QString)),
            this, SLOT(setLinePowerSuspendDelay(QString)));

    connect(m_powerInterface, SIGNAL(BatteryPlanChanged()),
            this, SLOT(initialData()));
    connect(m_batteryButtonGrid, SIGNAL(buttonChecked(QString)),
            SLOT(setBatteryPlan(QString)));

    connect(m_powerInterface, SIGNAL(BatteryIdleDelayChanged()),
            this, SLOT(initialData()));
    connect(m_batteryCustomExtendBoard->m_poweroffButtonGrid, SIGNAL(buttonChecked(QString)),
            this, SLOT(setBatteryIdleDelay(QString)));

    connect(m_powerInterface, SIGNAL(BatterySuspendDelayChanged()),
            this, SLOT(initialData()));
    connect(m_batteryCustomExtendBoard->m_standByButtonGrid, SIGNAL(buttonChecked(QString)),
            this, SLOT(setBatterySuspendDelay(QString)));
}

QFrame* Power::getContent()
{
    return m_frame;
}
