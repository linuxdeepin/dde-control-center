#include "wirelesspage.h"
#include "accesspointwidget.h"
#include "networkdevice.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "translucentframe.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

WirelessPage::WirelessPage(NetworkDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev),

      m_listGroup(new SettingsGroup),
      m_switchBtn(new SwitchWidget)
{
    m_switchBtn->setTitle(tr("Status"));

    m_listGroup->appendItem(m_switchBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_listGroup);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("WLAN"));

    connect(m_switchBtn, &SwitchWidget::checkedChanegd, [=](const bool enabled) { emit requestDeviceEnabled(m_device->path(), enabled); });
    connect(dev, &NetworkDevice::enableChanged, m_switchBtn, &SwitchWidget::setChecked);
    connect(dev, &NetworkDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(dev, &NetworkDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(dev, &NetworkDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    m_switchBtn->setChecked(dev->enabled());

    QTimer::singleShot(0, this, [=] { emit requestDeviceAPList(m_device->path()); });
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    const QString ssid = apInfo.value("Ssid").toString();

    AccessPointWidget *w = new AccessPointWidget;

    w->setAPName(ssid);

    m_apItems.insert(ssid, w);
    m_listGroup->appendItem(w);
}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    qDebug() << apInfo;
}

void WirelessPage::onAPRemoved(const QString &ssid)
{
    if (!m_apItems.contains(ssid))
        return;

    AccessPointWidget *w = m_apItems[ssid];

    m_apItems.remove(ssid);
    m_listGroup->removeItem(w);
    w->deleteLater();
}
