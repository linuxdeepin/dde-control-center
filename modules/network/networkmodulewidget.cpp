#include "networkmodulewidget.h"
#include "networkdevice.h"
#include "networkmodel.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"

#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::network;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget(),

      m_devicesLayout(new SettingsGroup),

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

    m_centeralLayout->addWidget(m_devicesLayout);
    m_centeralLayout->addWidget(connGroup);
    m_centeralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowInfomation);
    connect(m_vpnBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowVpnPage);
    connect(m_pppBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowPppPage);
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);

    onDeviceListChanged(model->devices());
}

void NetworkModuleWidget::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    // remove old widgets
    QList<SettingsItem *> removeList;
    const int itemCount = m_devicesLayout->itemCount();
    for (int i(0); i != itemCount; ++i)
        removeList.append(m_devicesLayout->getItem(i));
    for (auto const item : removeList)
        m_devicesLayout->removeItem(item);
    qDeleteAll(removeList);

    int wiredDevice = 0;
    int wirelessDevice = 0;
    for (auto const dev : devices)
        switch (dev->type())
        {
        case NetworkDevice::Wired:      ++wiredDevice;      break;
        case NetworkDevice::Wireless:   ++wirelessDevice;   break;
        default:;
        }

    int index = 0;

    // add wired device list
    int count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wired)
            continue;

        NextPageWidget *w = new NextPageWidget;
        initButtonsConnection(dev, w);

        if (wiredDevice < 2)
            w->setTitle(tr("Wired Network"));
        else
            w->setTitle(tr("Wired Network%1").arg(++count));

        m_devices[w] = dev;
        m_devicesLayout->insertItem(index, w);
        ++index;
    }

    // add wireless device list
    count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        NextPageWidget *w = new NextPageWidget;
        initButtonsConnection(dev, w);

        if (wirelessDevice < 2)
            w->setTitle(tr("Wireless Network"));
        else
            w->setTitle(tr("Wireless Network%1").arg(++count));

        m_devices[w] = dev;
        m_devicesLayout->insertItem(index, w);
        ++index;
    }
}

void NetworkModuleWidget::onNextPageClicked()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    NetworkDevice *dev = m_devices[w];
    Q_ASSERT(dev);

    emit requestShowDeviceDetail(dev);
}

void NetworkModuleWidget::initButtonsConnection(NetworkDevice *dev, NextPageWidget *w)
{
    connect(w, &NextPageWidget::clicked, this, &NetworkModuleWidget::onNextPageClicked);

    connect(dev, &NetworkDevice::statusChanged, w, [=](NetworkDevice::DeviceStatus stat) { w->setValue(QString::number(stat)); });

    w->setValue(QString::number(dev->status()));
}
