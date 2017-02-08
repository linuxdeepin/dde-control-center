#include "proxypage.h"
#include "networkmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "lineeditwidget.h"

#include <QVBoxLayout>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {

namespace network {

const QStringList ProxyMethodList = { "none", "manual", "auto" };

ProxyPage::ProxyPage(QWidget *parent)
    : ContentWidget(parent),

      m_manualWidget(new TranslucentFrame),
      m_autoWidget(new TranslucentFrame),

      m_proxyType(new DSegmentedControl)
{
    m_proxyType->addSegmented(tr("None"));
    m_proxyType->addSegmented(tr("Manual"));
    m_proxyType->addSegmented(tr("Auto"));

    m_httpAddr = new LineEditWidget;
    m_httpAddr->setTitle(tr("Http Proxy"));
    m_httpPort = new LineEditWidget;
    m_httpPort->setTitle(tr("Port"));

    m_httpsAddr = new LineEditWidget;
    m_httpsAddr->setTitle(tr("Https Proxy"));
    m_httpsPort = new LineEditWidget;
    m_httpsPort->setTitle(tr("Port"));

    m_ftpAddr = new LineEditWidget;
    m_ftpAddr->setTitle(tr("Ftp Proxy"));
    m_ftpPort = new LineEditWidget;
    m_ftpPort->setTitle(tr("Port"));

    m_socksAddr = new LineEditWidget;
    m_socksAddr->setTitle(tr("Socks Proxy"));
    m_socksPort = new LineEditWidget;
    m_socksPort->setTitle(tr("Port"));

    m_autoUrl = new LineEditWidget;
    m_autoUrl->setTitle(tr("Configuration URL"));

    SettingsGroup *httpGroup = new SettingsGroup;
    httpGroup->appendItem(m_httpAddr);
    httpGroup->appendItem(m_httpPort);

    SettingsGroup *httpsGroup = new SettingsGroup;
    httpsGroup->appendItem(m_httpsAddr);
    httpsGroup->appendItem(m_httpsPort);

    SettingsGroup *ftpGroup = new SettingsGroup;
    ftpGroup->appendItem(m_ftpAddr);
    ftpGroup->appendItem(m_ftpPort);

    SettingsGroup *socksGroup = new SettingsGroup;
    socksGroup->appendItem(m_socksAddr);
    socksGroup->appendItem(m_socksPort);

    SettingsGroup *autoGroup = new SettingsGroup;
    autoGroup->appendItem(m_autoUrl);

    QVBoxLayout *manualLayout = new QVBoxLayout;
    manualLayout->addWidget(httpGroup);
    manualLayout->addWidget(httpsGroup);
    manualLayout->addWidget(ftpGroup);
    manualLayout->addWidget(socksGroup);
    manualLayout->setMargin(0);
    manualLayout->setSpacing(10);

    QVBoxLayout *autoLayout = new QVBoxLayout;
    autoLayout->addWidget(autoGroup);
    autoLayout->setSpacing(10);
    autoLayout->setMargin(0);

    m_manualWidget->setLayout(manualLayout);
    m_autoWidget->setLayout(autoLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_proxyType);
    mainLayout->setAlignment(m_proxyType, Qt::AlignCenter);
    mainLayout->addWidget(m_manualWidget);
    mainLayout->addWidget(m_autoWidget);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setTitle(tr("System Proxy"));
    setContent(mainWidget);

    connect(m_proxyType, &DSegmentedControl::currentChanged, [=](const int index) { emit requestSetProxyMethod(ProxyMethodList[index]); });

    QTimer::singleShot(1, this, [=] { emit requestQueryProxyMethod(); });
}

void ProxyPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::proxyMethodChanged, this, &ProxyPage::onProxyMethodChanged);
}

void ProxyPage::onProxyMethodChanged(const QString &proxyMethod)
{
    const int index = ProxyMethodList.indexOf(proxyMethod);
    if (index == -1)
        return;

    m_proxyType->setCurrentIndex(index);
}

}

}
