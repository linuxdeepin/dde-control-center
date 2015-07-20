#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "vpnapplet.h"
#include "vpncomponent.h"
#include "widgets/switchbutton.h"

VPNApplet::VPNApplet(VPNComponent * host) :
    QFrame(),
    m_host(host),
    m_mainFrame(NULL)
{    
    updateContent();

    connect(m_host, &VPNComponent::vpnStatesChanged, this, &VPNApplet::updateContent);
}

void VPNApplet::updateContent()
{
    if (m_mainFrame) {
        m_mainFrame->setParent(NULL);
        m_mainFrame->deleteLater();
    } else {
        m_mainFrame = new QFrame(this);
        m_mainFrame->setFixedWidth(200);

        QVBoxLayout * mainLayout = new QVBoxLayout(m_mainFrame);

        QHBoxLayout * firstLineLayout = new QHBoxLayout;

        QLabel * vpnLabel = new QLabel("VPN");
        vpnLabel->setStyleSheet("QLabel { color: white }");

        SwitchButton * vpnEnabledSwitch = new SwitchButton;
        vpnEnabledSwitch->setChecked(m_host->m_enabled);

        connect(vpnEnabledSwitch, &SwitchButton::toggled, [=](bool checked){
            m_host->m_dbusNetwork->setVpnEnabled(checked);
        });

        firstLineLayout->addWidget(vpnLabel);
        firstLineLayout->addStretch();
        firstLineLayout->addWidget(vpnEnabledSwitch);

        mainLayout->addLayout(firstLineLayout);

        foreach (VPN vpn, m_host->m_vpns) {
            QHBoxLayout * layout = new QHBoxLayout;

            QLabel * vpnName = new QLabel(vpn.id);
            vpnName->setStyleSheet("QLabel { color: white }");

            layout->addWidget(vpnName);

            mainLayout->addLayout(layout);
        }

        m_mainFrame->adjustSize();
        this->adjustSize();
    }
}
