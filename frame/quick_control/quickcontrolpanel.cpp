#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"
#include "vpn/vpncontrolpage.h"

#include "network/networkmodel.h"
#include "network/networkworker.h"

#include <QVBoxLayout>

using namespace dcc;
using dcc::network::NetworkModel;
using dcc::network::NetworkWorker;

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    m_networkModel = new NetworkModel(this);
    m_networkWorker = new NetworkWorker(m_networkModel, this);

    m_itemStack->addWidget(new BasicSettingsPage);
    m_itemStack->addWidget(new QWidget);
    m_itemStack->addWidget(new VpnControlPage(m_networkModel));
    m_itemStack->addWidget(new QWidget);
    m_itemStack->addWidget(new QWidget);

    QuickSwitchButton *btSwitch = new QuickSwitchButton(1, "bluetooth");
    QuickSwitchButton *vpnSwitch = new QuickSwitchButton(2, "VPN");
    QuickSwitchButton *wifiSwitch = new QuickSwitchButton(3, "wifi");
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(4, "display");
    QuickSwitchButton *detailSwitch = new QuickSwitchButton(0, "all_settings");

    btSwitch->setObjectName("QuickSwitchBluetooth");
    btSwitch->setAccessibleName("QuickSwitchBluetooth");
    vpnSwitch->setObjectName("QuickSwitchVPN");
    vpnSwitch->setAccessibleName("QuickSwitchVPN");
    wifiSwitch->setObjectName("QuickSwitchWiFi");
    wifiSwitch->setAccessibleName("QuickSwitchWiFi");
    displaySwitch->setObjectName("QuickSwitchDisplay");
    displaySwitch->setAccessibleName("QuickSwitchDisplay");
    detailSwitch->setObjectName("QuickSwitchAllSettings");
    detailSwitch->setAccessibleName("QuickSwitchAllSettings");

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(btSwitch);
    btnsLayout->addWidget(vpnSwitch);
    btnsLayout->addWidget(wifiSwitch);
    btnsLayout->addWidget(displaySwitch);
    btnsLayout->addWidget(detailSwitch);
    btnsLayout->setContentsMargins(0, 15, 0, 15);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_itemStack);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(btSwitch, &QuickSwitchButton::hovered, this, &QuickControlPanel::pageSwitched);
    connect(btSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::switchToggled);
    connect(vpnSwitch, &QuickSwitchButton::hovered, this, &QuickControlPanel::pageSwitched);
    connect(vpnSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::switchToggled);
    connect(wifiSwitch, &QuickSwitchButton::hovered, this, &QuickControlPanel::pageSwitched);
    connect(wifiSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::switchToggled);
    connect(displaySwitch, &QuickSwitchButton::hovered, this, &QuickControlPanel::pageSwitched);
    connect(displaySwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::switchToggled);
    connect(detailSwitch, &QuickSwitchButton::hovered, this, &QuickControlPanel::pageSwitched);
    connect(detailSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::requestDetailConfig);

    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, vpnSwitch, &QuickSwitchButton::setChecked);
    connect(vpnSwitch, &QuickSwitchButton::checkedChanged, m_networkWorker, &NetworkWorker::setVpnEnable);

    vpnSwitch->setChecked(m_networkModel->vpnEnabled());
}

void QuickControlPanel::pageSwitched(const int index)
{
//    const int current = m_itemStack->currentIndex();
//    if (current == index)
//        return m_itemStack->setCurrentIndex(0);

    Q_ASSERT(m_itemStack->count() > index);
    m_itemStack->setCurrentIndex(index);
}

void QuickControlPanel::switchToggled(const int index)
{
    QuickSwitchButton *btn = static_cast<QuickSwitchButton *>(sender());
    Q_ASSERT(btn);

    qDebug() << btn << btn->checked() << index;
}
