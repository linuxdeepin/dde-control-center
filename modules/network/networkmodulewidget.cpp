#include "networkmodulewidget.h"
#include "networkdevice.h"
#include "networkmodel.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"

#include <QDebug>

using namespace dcc;
using namespace dcc::network;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget(),

      m_wiredLayout(new QVBoxLayout),
      m_wirelessLayout(new QVBoxLayout),

      m_pppBtn(new NextPageWidget),
      m_vpnBtn(new NextPageWidget),
      m_proxyBtn(new NextPageWidget),

      m_detailBtn(new NextPageWidget)
{
    setObjectName("Network");

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

    m_centeralLayout->addLayout(m_wiredLayout);
    m_centeralLayout->addLayout(m_wirelessLayout);
    m_centeralLayout->addWidget(connGroup);
    m_centeralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowDetailPage);
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::wiredDeviceListChanged, this, &NetworkModuleWidget::onWiredDeviceListChanged);

    onWiredDeviceListChanged(model->wiredDeviceList());
}

void NetworkModuleWidget::onWiredDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    qDebug() << devices.size();
}
