#include "wirelesspage.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "networkdevice.h"
#include "translucentframe.h"

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
    m_switchBtn->setChecked(dev->enabled());
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;
}
