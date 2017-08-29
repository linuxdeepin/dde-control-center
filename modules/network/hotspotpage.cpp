#include "hotspotpage.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "wirelessdevice.h"

#include <QVBoxLayout>

namespace dcc {

using namespace widgets;

namespace network {

HotspotPage::HotspotPage(WirelessDevice *wdev, QWidget *parent)
    : ContentWidget(parent),
      m_wdev(wdev)
{
    Q_ASSERT(m_wdev->supportHotspot());

    m_hotspotSwitch = new SwitchWidget;
    m_hotspotSwitch->setTitle("aaa");

    m_configureWidget = new NextPageWidget;
    m_configureWidget->setTitle(tr("Hotspot Settings"));

    SettingsGroup *sgrp = new SettingsGroup;
    sgrp->appendItem(m_hotspotSwitch);
    sgrp->appendItem(m_configureWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(sgrp);
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Hotspot"));
}

void HotspotPage::setModel(NetworkModel *model)
{
    m_model = model;
}

}

}
