#include "networkmodulewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"

using namespace dcc;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget(),

      m_pppBtn(new NextPageWidget),
      m_vpnBtn(new NextPageWidget),
      m_proxyBtn(new NextPageWidget),

      m_detailBtn(new NextPageWidget)
{
    m_pppBtn->setTitle(tr("DSL"));
    m_vpnBtn->setTitle(tr("VPN"));
    m_proxyBtn->setTitle(tr("Proxy"));

    m_detailBtn->setTitle(tr("Network Details"));

    SettingsGroup *connGroup = new SettingsGroup;
    connGroup->appendItem(m_pppBtn);
    connGroup->appendItem(m_vpnBtn);
    connGroup->appendItem(m_proxyBtn);

    SettingsGroup *detailGroup = new SettingsGroup;
    detailGroup->appendItem(m_detailBtn);

    m_centeralLayout->addWidget(connGroup);
    m_centeralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowDetailPage);
}
