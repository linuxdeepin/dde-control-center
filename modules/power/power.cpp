#include <QLabel>
#include <QDebug>

#include "power.h"
#include "constants.h"

Power::Power()
    :QObject(),
      m_contentHeight(30),
      m_bgContentHeight(60),
      m_frame(new QFrame),
      m_powerManagementFrame(new PowerManagement),
      m_powerInterfaceManagement(new PowerInterfaceManagement)

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
        m_closeLaptopActionFrame->show();
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
    m_powerPerformanceButtonGroup = new DButtonGrid(2, 2);

    m_powerPerformaceString << tr("Balanced") << tr("Power saver") << tr("High performance") << tr("Custom");
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
    layout->addWidget(m_pressPowerButtonActionFrame);
    layout->addWidget(m_closeLaptopActionFrame);
    layout->addLayout(lockWhenActiveLayout);
    layout->addLayout(powerConnectLayout);
    layout->addLayout(batteryUsedLayout);
    layout->addStretch();
    m_frame->setLayout(layout);
}
void Power::updatePowerManagermentUI() {
    ////////////////////////////////////////////////////--get battery setting
    m_batteryIsPresent = m_powerInterfaceManagement->getBatteryIsPresent();
    m_onBattery = m_powerInterfaceManagement->getBatteryon();
    m_batteryPercentage = m_powerInterfaceManagement->getBatteryPresent();
//    m_batteryIsPresent = true; //for testing
//    m_batteryPercentage = 60;
    m_powerManagementFrame->setElectricQuantity(m_batteryPercentage);
    m_powerManagementFrame->batteryReservedControl(m_batteryIsPresent);
}
void Power::updatePowerLockUI() {
    m_chooseNeedPasswdButton->setChecked(m_powerInterfaceManagement->getLockWhenActive());
}
void Power::updatePressPowerButtonReactionUI() {
    qint32 buttonId = m_powerInterfaceManagement->getPowerButtonAction();

    m_pressPowerButtonActionFrame->setPowerButtonAction(buttonId);
}
void Power::updatePressLidPowerButtonReactionUI() {
    qint32 buttonIndex = m_powerInterfaceManagement->getLidCloseAction();
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
    connect(m_powerInterfaceManagement, SIGNAL(BatteryPercentageChanged(double)),
            m_powerManagementFrame, SLOT(setElectricQuantity(double)));

    connect(m_powerInterfaceManagement, SIGNAL(BatteryPercentageChanged()),
            this, SLOT(updatePowerManagermentUI()));
    connect(m_powerInterfaceManagement, SIGNAL(BatteryIsPresentChanged()),
            this, SLOT(updatePowerManagermentUI()));

    connect(m_powerManagementFrame, SIGNAL(Reset()),
            m_powerInterfaceManagement, SLOT(Reset()));

    connect(m_powerInterfaceManagement, SIGNAL(LockWhenActiveChanged()),
            this, SLOT(updatePowerLockUI()));
    connect(m_chooseNeedPasswdButton, SIGNAL(checkedChanged(bool)),
            m_powerInterfaceManagement,
            SLOT(setLockWhenActive(bool)));

    connect(m_powerInterfaceManagement, SIGNAL(PowerButtonActionChanged()),
             SLOT(updatePressPowerButtonReactionUI()));
    connect(m_pressPowerButtonActionFrame, SIGNAL(powerButtonAction(QString)),
            m_powerInterfaceManagement, SLOT(setPowerButtonAction(QString)));
    connect(m_powerInterfaceManagement, SIGNAL(LidClosedActionChanged()),
              SLOT(updatePressLidPowerButtonReactionUI()));

    connect(m_closeLaptopActionFrame, SIGNAL(powerButtonAction(QString)),
            m_powerInterfaceManagement, SLOT(setLidCloseAction(QString)));
    connect(m_powerInterfaceManagement, SIGNAL(LinePowerPlanChanged()),
            this, SLOT(updateLinePowerPlanUI()));

    connect(m_powerPerformanceButtonGroup, SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setLinePowerPlan(QString)));
    connect(m_powerPerformanceButtonGroup, SIGNAL(buttonChecked(QString)),
            SLOT(setConnectPowerExpand(QString)));

    connect(m_powerPerformanceButtonGroup, SIGNAL(buttonMouseEntered(QString)),
            this, SLOT(showPowerTooltip(QString)));
    connect(m_powerPerformanceButtonGroup, SIGNAL(buttonMouseLeaved(QString)),
            this, SLOT(hidePowerTooltip(QString)));

    connect(m_powerInterfaceManagement, SIGNAL(LinePowerIdleDelayChanged()),
            this, SLOT(updateLinePowerIdleDelayUI()));
    connect(m_powerCustomExtendBoard->m_poweroffButtonGrid,
            SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setLinePowerIdleDelay(QString)));

    connect(m_powerInterfaceManagement, SIGNAL(LinePowerSuspendDelayChanged()),
            this, SLOT(updateLinePowerSuspendDelayUI()));
    connect(m_powerCustomExtendBoard->m_suspendButtonGrid,
            SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setLinePowerSuspendDelay(QString)));

    connect(m_powerInterfaceManagement, SIGNAL(BatteryPlanChanged()),
            this, SLOT(updateBatteryPlanUI()));
    connect(m_batteryButtonGrid, SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setBatteryPlan(QString)));

    connect(m_batteryButtonGrid, SIGNAL(buttonMouseEntered(QString)),
            this, SLOT(showBatteryTooltip(QString)));

    connect(m_batteryButtonGrid, SIGNAL(buttonMouseLeaved(QString)),
            this, SLOT(hideBatteryTooltip(QString)));

    connect(m_batteryButtonGrid, SIGNAL(buttonChecked(QString)),
            SLOT(setUseBatteryExpand(QString)));
    connect(m_powerInterfaceManagement, SIGNAL(BatteryIdleDelayChanged()),
            this, SLOT(updateBatteryIdleDelayUI()));
    connect(m_batteryCustomExtendBoard->m_poweroffButtonGrid, SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setBatteryIdleDelay(QString)));

    connect(m_powerInterfaceManagement, SIGNAL(BatterySuspendDelayChanged()),
            this, SLOT(updateBatterySuspendDelayUI()));
    connect(m_batteryCustomExtendBoard->m_suspendButtonGrid, SIGNAL(buttonChecked(QString)),
            m_powerInterfaceManagement, SLOT(setBatterySuspendDelay(QString)));
}

QFrame* Power::getContent()
{
    return m_frame;
}
