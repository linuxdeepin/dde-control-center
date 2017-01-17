#include "proxypage.h"
#include "networkmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "lineeditwidget.h"

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {

namespace network {

ProxyPage::ProxyPage(QWidget *parent)
    : ContentWidget(parent),

      m_manualWidget(new TranslucentFrame),
      m_autoWidget(new TranslucentFrame),

      m_proxyType(new DSegmentedControl)
{
    m_proxyType->addSegmented(tr("None"));
    m_proxyType->addSegmented(tr("Manual"));
    m_proxyType->addSegmented(tr("Auto"));

    SettingsGroup *httpProxy = new SettingsGroup;
    httpProxy->setHeaderVisible(true);
    httpProxy->headerItem()->setTitle(tr("HTTP Proxy"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_proxyType);
    mainLayout->setAlignment(m_proxyType, Qt::AlignCenter);
    mainLayout->addWidget(m_manualWidget);
    mainLayout->addWidget(m_autoWidget);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setTitle(tr("System Proxy"));
    setContent(mainWidget);
}

void ProxyPage::setModel(NetworkModel *model)
{
    m_model = model;
}

}

}
