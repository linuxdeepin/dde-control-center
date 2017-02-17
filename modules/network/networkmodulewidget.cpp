#include "networkmodulewidget.h"
#include "networkdevice.h"
#include "networkmodel.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"

#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::network;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget(),

      m_devicesLayout(new QVBoxLayout),

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

    m_devicesLayout->setMargin(0);
    m_devicesLayout->setSpacing(10);

    SettingsGroup *connGroup = new SettingsGroup;
    connGroup->appendItem(m_pppBtn);
    connGroup->appendItem(m_vpnBtn);
    connGroup->appendItem(m_proxyBtn);

    SettingsGroup *detailGroup = new SettingsGroup;
    detailGroup->appendItem(m_detailBtn);

    m_centralLayout->addLayout(m_devicesLayout);
    m_centralLayout->addWidget(connGroup);
    m_centralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowInfomation);
    connect(m_vpnBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowVpnPage);
    connect(m_pppBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowPppPage);
    connect(m_proxyBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowProxyPage);
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);

    onDeviceListChanged(model->devices());
}

void NetworkModuleWidget::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    bool recreate = false;
    const auto devs = m_devices.values().toSet();

    if (devs.size() != devices.size())
    {
        recreate = true;
    } else {
        for (auto *dev : devices)
        {
            if (!devs.contains(dev))
                recreate = true;
            break;
        }
    }
    if (!recreate)
        return;

    // remove old widgets
    while (QLayoutItem *item = m_devicesLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }
    m_devices.clear();

    int wiredDevice = 0;
    int wirelessDevice = 0;
    for (auto const dev : devices)
        switch (dev->type())
        {
        case NetworkDevice::Wired:      ++wiredDevice;      break;
        case NetworkDevice::Wireless:   ++wirelessDevice;   break;
        default:;
        }

    // add wired device list
    int count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wired)
            continue;

        createDeviceGroup(dev, ++count, wiredDevice > 1);
    }

    // add wireless device list
    count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        createDeviceGroup(dev, ++count, wirelessDevice > 1);
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

void NetworkModuleWidget::createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple)
{
    SettingsGroup *g = new SettingsGroup;
    SwitchWidget *s = new SwitchWidget;
    NextPageWidget *w = new NextPageWidget;

    connect(s, &SwitchWidget::checkedChanged, [=](const bool checked) { emit requestDeviceEnable(dev->path(), checked); });
    connect(w, &NextPageWidget::clicked, this, &NetworkModuleWidget::onNextPageClicked);
    connect(dev, &NetworkDevice::enableChanged, s, &SwitchWidget::setChecked);
    connect(dev, &NetworkDevice::enableChanged, w, &NextPageWidget::setVisible);
    connect(dev, static_cast<void (NetworkDevice::*)(const QString &) const>(&NetworkDevice::statusChanged), w, &NextPageWidget::setValue, Qt::QueuedConnection);

    if (dev->type() == NetworkDevice::Wired)
    {
        if (multiple)
        {
            s->setTitle(tr("Wired Network Card%1").arg(number));
            w->setTitle(tr("Wired Network%1").arg(number));
        }
        else
        {
            s->setTitle(tr("Wired Network Card"));
            w->setTitle(tr("Wired Network"));
        }
    }
    else if (dev->type() == NetworkDevice::Wireless)
    {
        if (multiple)
        {
            s->setTitle(tr("Wireless Network Card%1").arg(number));
            w->setTitle(tr("Wireless Network%1").arg(number));
        }
        else
        {
            s->setTitle(tr("Wireless Network Card"));
            w->setTitle(tr("Wireless Network"));
        }
    }

    const bool devEnabled = dev->enabled();
    s->setChecked(devEnabled);
    w->setVisible(devEnabled);
    w->setValue(dev->statusString());

    g->appendItem(s);
    g->appendItem(w);

    m_devices[w] = dev;
    m_devicesLayout->addWidget(g);
}
