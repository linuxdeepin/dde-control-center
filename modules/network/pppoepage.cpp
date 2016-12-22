#include "pppoepage.h"
#include "settingsgroup.h"
#include "networkmodel.h"
#include "wireddevice.h"
#include "settingsheaderitem.h"
#include "translucentframe.h"
#include "nextpagewidget.h"

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent),
      m_grpsLayout(new QVBoxLayout)
{
    m_grpsLayout->setSpacing(10);
    m_grpsLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_grpsLayout);

    setContent(mainWidget);
    setTitle(tr("PPP"));
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    int index = 0;
    for (auto *dev : m_model->devices())
    {
        if (dev->type() != NetworkDevice::Wired)
            continue;
        WiredDevice *wDev = static_cast<WiredDevice *>(dev);

        SettingsGroup *grp = new SettingsGroup;
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(tr("Wired network card%1").arg(++index));

        for (const auto &info : wDev->pppConnections())
        {
            NextPageWidget *w = new NextPageWidget;
            w->setTitle(info.value("Id").toString());
            grp->appendItem(w);
        }

        m_grpsLayout->addWidget(grp);
    }
}
