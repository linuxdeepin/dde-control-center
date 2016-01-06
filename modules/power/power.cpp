#include <QLabel>
#include <QDebug>

#include <libdui/dbuttongrid.h>

#include "power.h"
#include "constants.h"

Power::Power()
    :QObject(),
      m_contentHeight(30),
      m_bgContentHeight(60),
      m_frame(new QFrame),
      m_powerManagementFrame(new PowerManagement),
      m_powerInter(new PowerInterface)

{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Power Management"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    initUI();
    initData();
    initConnection();
}

Power::~Power()
{
    qDebug() << "~Power()";
    m_frame->deleteLater();

}

void Power::updateBatteryUsedControlUI() {
    if (!m_batteryIsPresent) {
        m_closeLaptopActionFrame->hide();
        m_batterySettingDBaseLine->hide();
        m_batterySettingExpand->hide();
        m_batteryCustomExtendBoard->hide();
    } else {
#ifndef DCC_DISABLE_POWER_ACTION
        m_closeLaptopActionFrame->show();
#endif
        m_batterySettingDBaseLine->show();
        m_batterySettingExpand->show();
        m_batteryCustomExtendBoard->show();
    }
}

void Power::initClockWhenActiveUI()
{
    m_chooseNeedPasswdLine = new DHeaderLine;
    m_chooseNeedPasswdLine->setTitle(tr("Password required to wake up"));
    m_chooseNeedPasswdButton = new DSwitchButton;
    m_chooseNeedPasswdLine->setContent(m_chooseNeedPasswdButton);
    m_chooseNeedPasswdLine->setFixedHeight(m_contentHeight);
    m_thirdHSeparator = new DSeparatorHorizontal;

    lockWhenActiveLayout = new QVBoxLayout;
    lockWhenActiveLayout->setMargin(0);
    lockWhenActiveLayout->setSpacing(0);
    lockWhenActiveLayout->addWidget(m_chooseNeedPasswdLine);
    lockWhenActiveLayout->addWidget(m_thirdHSeparator);
}

void Power::initPowerConnectionPanelUI() {
    m_powerSettingDBaseLine = new DBaseLine;
    QLabel* breatingPowerTotalLabel = new QLabel;
    m_powerConnectionBreathingLabel = new DBreathingLabel;
    m_powerConnectionBreathingLabel->setColor(DCC::TextNormalColor);
    m_powerConnectionBreathingLabel->setText(tr("Plugged in"));
    breatingPowerTotalLabel->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Expanding);

    QVBoxLayout* powerBreathingLayout = new QVBoxLayout;
    powerBreathingLayout->setMargin(0);
    powerBreathingLayout->setSpacing(0);
    powerBreathingLayout->addStretch();
    powerBreathingLayout->addWidget(m_powerConnectionBreathingLabel);
    powerBreathingLayout->addSpacing(5);
    breatingPowerTotalLabel->setLayout(powerBreathingLayout);

    m_powerSettingDBaseLine->setLeftContent(breatingPowerTotalLabel);

    m_powerSettingExpand = new DBaseExpand;
    m_powerSettingExpand->setExpandedSeparatorVisible(false);
    m_powerSettingExpand->setFixedHeight(m_bgContentHeight);
    m_powerSettingExpand->setHeader(m_powerSettingDBaseLine);
    m_powerSettingExpand->setHeaderHeight(30);

    m_powerSettingDBaseLine->setFixedHeight(60);
    m_powerDynamicLabel = new DynamicLabel(m_powerSettingDBaseLine);
    m_powerDynamicLabel->setFixedSize(310, 55);
    m_powerDynamicLabel->label()->setFixedHeight(55);
    m_powerDynamicLabel->label()->setAlignment(Qt::AlignBottom);
    m_powerDynamicLabel->label()->setWordWrap(true);
    m_powerDynamicLabel->setHideDuration(200);
    m_powerDynamicLabel->setShowDuration(500);
    m_linePowerButtonGrid = new DButtonGrid(2, 2);

    m_powerPerformaceString << tr("Balanced") << tr("Power saver") << tr("High performance") << tr("Custom");
    m_linePowerButtonGrid->addButtons(m_powerPerformaceString);
    m_linePowerButtonGrid->setItemSize(150, 30);
    m_linePowerButtonGrid->checkButtonByIndex(2);
    m_powerSettingExpand->setContent(m_linePowerButtonGrid);
    m_powerSettingExpand->setExpand(true);
/////////////////////////////////////////////////////-- power connection custom setting expand
    m_powerCustomExtendBoard = new DExtendBoard;
    m_powerCustomExtendBoard->setExpand(true);

    powerConnectLayout = new QVBoxLayout;
    powerConnectLayout->setMargin(0);
    powerConnectLayout->setSpacing(0);
    powerConnectLayout->addWidget(m_powerSettingDBaseLine);
    powerConnectLayout->addWidget(m_powerSettingExpand);
    powerConnectLayout->addWidget(m_powerCustomExtendBoard);

}
void Power::initBatteryUsedUI() {
    /////////////////////////////////////////////////////////--use battery setting panel
    m_batterySettingDBaseLine = new DBaseLine;

    QLabel* breatingBatteryTotalLabel = new QLabel;
    breatingBatteryTotalLabel->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Expanding);

    m_batteryBreathingLabel = new DBreathingLabel;
    m_batteryBreathingLabel->setText(tr("On battery"));
    m_batteryBreathingLabel->setColor(DCC::TextNormalColor);

    QVBoxLayout* batteryBreathingLayout = new QVBoxLayout;
    batteryBreathingLayout->setMargin(0);
    batteryBreathingLayout->setSpacing(0);
    batteryBreathingLayout->addStretch();
    batteryBreathingLayout->addWidget(m_batteryBreathingLabel);
    batteryBreathingLayout->addSpacing(5);
    breatingBatteryTotalLabel->setLayout(batteryBreathingLayout);


    m_batterySettingExpand = new DBaseExpand;
    m_batterySettingExpand->setExpandedSeparatorVisible(false);
    m_batterySettingDBaseLine->setLeftContent(breatingBatteryTotalLabel);
    m_batterySettingDBaseLine->setFixedHeight(60);
    m_batteryDynamicLabel = new DynamicLabel(m_batterySettingDBaseLine);
    m_batteryDynamicLabel->setFixedSize(310, 55);
    m_batteryDynamicLabel->label()->setFixedHeight(55);
    m_batteryDynamicLabel->label()->setAlignment(Qt::AlignBottom);
    m_batteryDynamicLabel->label()->setWordWrap(true);
    m_batteryDynamicLabel->setHideDuration(200);
    m_batteryDynamicLabel->setShowDuration(500);

    m_batterySettingExpand->setFixedHeight(m_bgContentHeight);
    m_batterySettingExpand->setHeader(m_batterySettingDBaseLine);
    m_batterySettingExpand->setHeaderHeight(60);
    m_batteryButtonGrid = new DButtonGrid(2, 2);
    m_powerPerformaceString << tr("Balanced") << tr("Power saver") << tr("High performance") << tr("Custom");
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
    batteryUsedLayout->addWidget(m_batterySettingDBaseLine);
    batteryUsedLayout->addWidget(m_batterySettingExpand);
    batteryUsedLayout->addWidget(m_batteryCustomExtendBoard);


    QTimer* t=new QTimer(this);
    t->setSingleShot(true);
    t->setInterval(100);
    connect(t,SIGNAL(timeout()), this, SLOT(handleTestTimer()));
    t->start();

}

void Power::handleTestTimer(){
    int index = m_batteryButtonGrid->getCurrentCheckedIndex();
    m_batteryButtonGrid->checkButtonByIndex(3-index);
    m_batteryButtonGrid->checkButtonByIndex(index);
}

void Power::initUI() {

    linePowerAction  << tr("Shutdown") << tr("Suspend") << tr("Ask");
    m_pressPowerButtonActionFrame = new PressPowerButtonAction(linePowerAction);
    m_pressPowerButtonActionFrame->setTitle(tr("When I press the power button"));

    closeLaptopAction << tr("Shutdown") << tr("Suspend") << tr("Nothing");
    m_closeLaptopActionFrame = new PressPowerButtonAction(closeLaptopAction);
    m_closeLaptopActionFrame->setTitle(tr("When I close the lid"));

    initClockWhenActiveUI();
    initPowerConnectionPanelUI();
    initBatteryUsedUI();
    QVBoxLayout * layout = new QVBoxLayout(m_frame);
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_powerManagementFrame);
#ifndef DCC_DISABLE_POWER_ACTION
    layout->addWidget(m_pressPowerButtonActionFrame);
    layout->addWidget(m_closeLaptopActionFrame);
#endif
    layout->addLayout(lockWhenActiveLayout);
    layout->addLayout(powerConnectLayout);
    layout->addLayout(batteryUsedLayout);
    layout->addStretch();
    m_frame->setLayout(layout);
}
void Power::updatePowerManagermentUI() {
    ////////////////////////////////////////////////////--get battery setting
    m_batteryIsPresent = m_powerInter->getBatteryIsPresent();
    m_onBattery = m_powerInter->getBatteryon();
    m_batteryPercentage = m_powerInter->getBatteryPresent();

//    m_batteryIsPresent = true; //for testing
//    m_batteryPercentage = 100;

    m_powerManagementFrame->setElectricQuantity(m_batteryPercentage);
    m_powerManagementFrame->batteryReservedControl(m_batteryIsPresent);
}
void Power::updatePowerLockUI() {
    m_chooseNeedPasswdButton->setChecked(m_powerInter->getLockWhenActive());
}
void Power::updatePressPowerButtonReactionUI() {
    qint32 buttonId = m_powerInter->getPowerButtonAction();

    m_pressPowerButtonActionFrame->setPowerButtonAction(buttonId);
}
void Power::updatePressLidPowerButtonReactionUI() {
    qint32 buttonIndex = m_powerInter->getLidCloseAction();
    m_closeLaptopActionFrame->setPowerButtonAction(buttonIndex);
}
void Power::initData() {
    updatePowerManagermentUI();
    updatePowerLockUI();
    ////////////////////////////////////////////////////--get press power button action panel
    updatePressPowerButtonReactionUI();
    updatePressLidPowerButtonReactionUI();
    ////////////////////////////////////////////////////--getLinePowerPlan
    updateLinePowerPlanUI();
    updateBatteryPlanUI();
    ////////////////////////////////////////////////////-- lock when Active
    updateBatteryUsedControlUI();
}

void Power::set4ButtonGridChecked(int idIndex, DButtonGrid *buttonGroup) {
    switch(idIndex) {
        case 0: { buttonGroup->checkButtonByIndex(3);break;}
        case 1: { buttonGroup->checkButtonByIndex(1);break;}
        case 2: { buttonGroup->checkButtonByIndex(0);break;}
        default: { buttonGroup->checkButtonByIndex(2);}
    }
}
void Power::set7ButtonGridChecked(int idIndex, DButtonGrid* buttonGroup) {
    switch(idIndex) {
        case 1: { buttonGroup->checkButtonByIndex(0); break;}
        case 5: { buttonGroup->checkButtonByIndex(1); break;}
        case 10: { buttonGroup->checkButtonByIndex(2); break;}
        case 15: { buttonGroup->checkButtonByIndex(3); break;}
        case 30: { buttonGroup->checkButtonByIndex(4); break;}
        case 60: { buttonGroup->checkButtonByIndex(5); break;}
        default: { buttonGroup->checkButtonByIndex(6);}
    }
}
void Power::initConnection() {
///////////////update the battery status in powerManagement head title
    connect(m_powerInter, SIGNAL(BatteryPercentageChanged(double)), m_powerManagementFrame, SLOT(setElectricQuantity(double)));
    connect(m_powerInter, SIGNAL(BatteryPercentageChanged()), this, SLOT(updatePowerManagermentUI()));
    connect(m_powerInter, SIGNAL(BatteryIsPresentChanged()), this, SLOT(updatePowerManagermentUI()));

///////////////Reset the data of power module
    connect(m_powerManagementFrame, SIGNAL(Reset()), m_powerInter, SLOT(Reset()));

//////////////Wake up need password or not
    connect(m_powerInter, SIGNAL(LockWhenActiveChanged()), this, SLOT(updatePowerLockUI()));
    connect(m_chooseNeedPasswdButton, SIGNAL(checkedChanged(bool)), m_powerInter, SLOT(setLockWhenActive(bool)));

/////////////Set the action of press power button
    connect(m_powerInter, SIGNAL(PowerButtonActionChanged()), SLOT(updatePressPowerButtonReactionUI()));
    connect(m_pressPowerButtonActionFrame, SIGNAL(powerButtonIndexChanged(int)), m_powerInter, SLOT(setPowerButtonAction(int)));
////////////Close the lid of laptop
    connect(m_powerInter, SIGNAL(LidClosedActionChanged()), SLOT(updatePressLidPowerButtonReactionUI()));
    connect(m_closeLaptopActionFrame, SIGNAL(powerButtonIndexChanged(int)), m_powerInter, SLOT(setLidCloseAction(int)));

/////////////Set the linePowerPlan
    connect(m_linePowerButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)), m_powerInter, SLOT(setLinePowerPlan(int)));
    connect(m_powerInter, SIGNAL(LinePowerPlanChanged()), this, SLOT(updateLinePowerPlanUI()));

    connect(m_linePowerButtonGrid, SIGNAL(buttonEnteredIndexChanged(int)), this, SLOT(showPowerTooltip(int)));
    connect(m_linePowerButtonGrid, SIGNAL(buttonLeavedIndexChanged(int)), this, SLOT(hidePowerTooltip(int)));
    ////////////////set the idleDelay&suspendDelay in linePowerPlan
    connect(m_powerInter, SIGNAL(LinePowerIdleDelayChanged()), this, SLOT(updateLinePowerIdleDelayUI()));
    connect(m_powerInter, SIGNAL(LinePowerSuspendDelayChanged()), this, SLOT(updateLinePowerSuspendDelayUI()));

    connect(m_powerCustomExtendBoard->m_idleButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)),
            m_powerInter, SLOT(setLinePowerIdleDelay(int)));
    connect(m_powerCustomExtendBoard->m_suspendButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)),
              m_powerInter, SLOT(setLinePowerSuspendDelay(int)));

///////////////Set the batteryPlan
    connect(m_batteryButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)), m_powerInter, SLOT(setBatteryPlan(int)));
    connect(m_powerInter, SIGNAL(BatteryPlanChanged()), this, SLOT(updateBatteryPlanUI()));

    connect(m_batteryButtonGrid, SIGNAL(buttonEnteredIndexChanged(int)), this, SLOT(showBatteryTooltip(int)));
    connect(m_batteryButtonGrid, SIGNAL(buttonLeavedIndexChanged(int)), this, SLOT(hideBatteryTooltip(int)));

    ////////////////Set the idleDelay&suspendDelay in batteryPlan
    connect(m_powerInter, SIGNAL(BatterySuspendDelayChanged()), this, SLOT(updateBatterySuspendDelayUI()));
    connect(m_powerInter, SIGNAL(BatteryIdleDelayChanged()), this, SLOT(updateBatteryIdleDelayUI()));

    connect(m_batteryCustomExtendBoard->m_idleButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)),
            m_powerInter, SLOT(setBatteryIdleDelay(int)));

    connect(m_batteryCustomExtendBoard->m_suspendButtonGrid, SIGNAL(buttonCheckedIndexChanged(int)),
            m_powerInter, SLOT(setBatterySuspendDelay(int)));
}

QFrame* Power::getContent()
{
    return m_frame;
}
