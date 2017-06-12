#include "wiredpage.h"
#include "wireddevice.h"
#include "networkmodel.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"

#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>

namespace dcc {

namespace network {

using namespace widgets;

WiredPage::WiredPage(WiredDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev)
{
    m_settingsGrp = new SettingsGroup;
    m_settingsGrp->setHeaderVisible(true);
    m_settingsGrp->headerItem()->setTitle("Settings List");

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addSpacing(10);
    centralLayout->addWidget(m_settingsGrp);
    centralLayout->addStretch();
    centralLayout->setSpacing(10);
    centralLayout->setMargin(0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Choose Settings"));
}

void WiredPage::setModel(NetworkModel *model)
{
    m_model = model;

//    int index = 0;
//    int count = 0;

//    const auto devices = m_model->devices();
//    for (int i(0); i != devices.size(); ++i)
//    {
//        if (devices[i]->type() != NetworkDevice::Wired)
//            continue;

//        ++count;
//        if (devices[i] == m_device)
//            index = i;
//    }

//    if (count == 1)
//        setTitle(tr("Wired Network Card"));
//    else
//        setTitle(tr("Wired Network Card%1").arg(index + 1));

    QTimer::singleShot(1, this, &WiredPage::initUI);
}

void WiredPage::initUI()
{
    const auto conns = m_device->connections();

    for (const auto c : conns)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(m_model->connectionNameByPath(c.path()));

        m_settingsGrp->appendItem(w);
    }
}

}

}
