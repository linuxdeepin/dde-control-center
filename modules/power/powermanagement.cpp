#include <QtWidgets/QFrame>

#include "constants.h"
#include "powermanagement.h"

PowerManagement::PowerManagement(QWidget *parent)
    : QFrame(parent)
{

    m_batteryIsPresent = false;
    m_batteryPercentage = 0;

    /*m_powerManagerViewLabel = new QLabel;
    m_powerManagerViewLabel->setFixedSize(300, 50);
    m_powerManagerViewLabel->setStyleSheet(QString("background-color: %1").arg(DCC::BgLightColor.name()));
    m_powerManagerLabel = new QLabel;
    m_powerManagerLabel->setText(tr("PowerManager"));
    m_powerManagerLabel->setStyleSheet("color: rgb(180, 180, 180)");
    m_shortSeparatorLine = new QLabel;
    m_shortSeparatorLine->setFixedSize(5, 2);
    m_shortSeparatorLine->setStyleSheet("background-color: rgb(79, 79, 79)");
    m_powerPercentageLabel = new QLabel;

    m_powerPercentageLabel->setText(QString("%1%").arg(m_batteryPercentage));
    m_powerPercentageLabel->setStyleSheet("color: rgb(180, 180, 180)");

    m_powerResetButton = new DTextButton(tr("Reset"));
    m_powerManagerLayout = new QHBoxLayout;
    m_powerManagerLayout->setMargin(0);
    m_powerManagerLayout->setSpacing(0);
    m_powerManagerLayout->addSpacing(14);
    m_powerManagerLayout->addWidget(m_powerManagerLabel);
    m_powerManagerLayout->addWidget(m_shortSeparatorLine);
    m_powerManagerLayout->addWidget(m_powerPercentageLabel);
    m_powerManagerLayout->addStretch();
    m_powerManagerLayout->addWidget(m_powerResetButton);
    m_powerManagerViewLabel->setLayout(m_powerManagerLayout);*/


    m_topHeaderLine = new ModuleHeader(tr("PowerManagement"));
    m_firstHSeparator = new DSeparatorHorizontal;

    topHeaderLayout = new QVBoxLayout;
    topHeaderLayout->setMargin(0);
    topHeaderLayout->setSpacing(0);
    topHeaderLayout->addWidget(m_topHeaderLine);
    topHeaderLayout->addWidget(m_firstHSeparator);
    setLayout(topHeaderLayout);

    initConnection();

}
PowerManagement::~PowerManagement() {
}

void PowerManagement::initConnection() {
    connect(m_topHeaderLine, SIGNAL(resetButtonClicked()), this, SLOT(reset()));
}
void PowerManagement::reset() {
    emit Reset();
}

void PowerManagement::setElectricQuantity(double electricQuantity) {
    m_batteryPercentage = electricQuantity;
}
void PowerManagement::batteryReservedControl(bool batteryIsPresent) {

    m_batteryIsPresent = batteryIsPresent;
    if (!m_batteryIsPresent) {
        headTitle = QString("PowerManagement");
    } else {
        headTitle = QString("PowerManagement-%1").arg(m_batteryPercentage);
    }
}
