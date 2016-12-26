#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"

#include <QVBoxLayout>

using namespace dcc;

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    m_itemStack->addWidget(new BasicSettingsPage);

    QuickSwitchButton *btSwitch = new QuickSwitchButton(0, "bluetooth");
    QuickSwitchButton *vpnSwitch = new QuickSwitchButton(1, "VPN");
    QuickSwitchButton *wifiSwitch = new QuickSwitchButton(2, "wifi");
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(3, "display");
    QuickSwitchButton *detailSwitch = new QuickSwitchButton(4, "all_settings");

    btSwitch->setObjectName("QuickSwitchBluetooth");
    vpnSwitch->setObjectName("QuickSwitchVPN");
    wifiSwitch->setObjectName("QuickSwitchWiFi");
    displaySwitch->setObjectName("QuickSwitchDisplay");
    detailSwitch->setObjectName("QuickSwitchAllSettings");

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(btSwitch);
    btnsLayout->addWidget(vpnSwitch);
    btnsLayout->addWidget(wifiSwitch);
    btnsLayout->addWidget(displaySwitch);
    btnsLayout->addWidget(detailSwitch);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_itemStack);
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(detailSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::requestDetailConfig);
}
