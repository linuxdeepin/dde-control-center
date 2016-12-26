#include "vpnpage.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "translucentframe.h"
#include "networkmodel.h"
#include "nextpagewidget.h"
#include "connectioneditpage.h"
#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>

using namespace dcc::widgets;
using namespace dcc::network;

VpnPage::VpnPage(QWidget *parent)
    : ContentWidget(parent),

      m_vpnSwitch(new SwitchWidget),
      m_vpnGroup(new SettingsGroup)
{
    m_vpnSwitch->setTitle(tr("VPN Status"));

    QPushButton *createVpnBtn = new QPushButton;
    createVpnBtn->setText(tr("Create VPN"));

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_vpnSwitch);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(switchGrp);
    mainLayout->addWidget(m_vpnGroup);
    mainLayout->addWidget(createVpnBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("VPN"));

    connect(m_vpnSwitch, &SwitchWidget::checkedChanegd, this, &VpnPage::requestVpnEnabled);
}

VpnPage::~VpnPage()
{
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void VpnPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &SwitchWidget::setChecked);
    connect(m_model, &NetworkModel::unhandledConnectionSessionCreated, this, &VpnPage::onVpnSessionCreated);

    m_vpnSwitch->setChecked(m_model->vpnEnabled());

    refershVpnList(m_model->vpns());
}

void VpnPage::refershVpnList(const QList<QJsonObject> &vpnList)
{
    m_vpnGroup->clear();
    qDeleteAll(m_vpns.keys());
    m_vpns.clear();

    for (const auto &vpn : vpnList)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(vpn.value("Id").toString());

        connect(w, &NextPageWidget::clicked, this, &VpnPage::onVpnClicked);

        m_vpns[w] = vpn;
        m_vpnGroup->appendItem(w);
    }
}

void VpnPage::onVpnClicked()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_vpns.contains(w));

    const QString connPath = m_vpns[w].value("Path").toString();

    emit requestEditVpn("/", connPath);
}

void VpnPage::onVpnSessionCreated(const QString &device, const QString &sessionPath)
{
    Q_ASSERT(device == "/");
    Q_ASSERT(m_editPage.isNull());

    m_editPage = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_editPage);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(sessionPath, sessionModel, m_editPage);

    m_editPage->setModel(sessionModel);
    connect(m_editPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(m_editPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(m_editPage, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &VpnPage::requestNextPage);

    emit requestNextPage(m_editPage);
}
