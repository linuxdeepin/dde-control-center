#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"

#include <QVBoxLayout>

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    m_itemStack->addWidget(new BasicSettingsPage);

    QuickSwitchButton *btSwitch = new QuickSwitchButton(0);
    QuickSwitchButton *vpnSwitch = new QuickSwitchButton(1);
    QuickSwitchButton *wifiSwitch = new QuickSwitchButton(2);
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(3);
    QuickSwitchButton *detailSwitch = new QuickSwitchButton(4);

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
}
