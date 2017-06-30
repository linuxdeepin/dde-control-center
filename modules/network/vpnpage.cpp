#include "vpnpage.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "translucentframe.h"
#include "networkmodel.h"
#include "optionitem.h"
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
      m_vpnGroup(new SettingsGroup),
      m_vpnTypePage(nullptr)
{
    m_vpnSwitch->setTitle(tr("VPN Status"));

    QPushButton *createVpnBtn = new QPushButton;
    createVpnBtn->setText(tr("Create VPN"));

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_vpnSwitch);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(switchGrp);
    mainLayout->addWidget(m_vpnGroup);
    mainLayout->addWidget(createVpnBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("VPN"));

    connect(m_vpnSwitch, &SwitchWidget::checkedChanged, this, &VpnPage::requestVpnEnabled);
    connect(createVpnBtn, &QPushButton::clicked, this, &VpnPage::createVPNSession);
}

VpnPage::~VpnPage()
{
    Q_ASSERT(!m_vpnTypePage);

    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void VpnPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &SwitchWidget::setChecked);
    connect(m_model, &NetworkModel::unhandledConnectionSessionCreated, this, &VpnPage::onVpnSessionCreated);
    connect(m_model, &NetworkModel::activeConnInfoChanged, this, &VpnPage::onActiveConnsInfoChanged);
    connect(m_model, &NetworkModel::connectionListChanged, this, [=] { refershVpnList(m_model->vpns()); });

    m_vpnSwitch->setChecked(m_model->vpnEnabled());

    refershVpnList(m_model->vpns());
}

void VpnPage::refershVpnList(const QList<QJsonObject> &vpnList)
{
    // NOTE: vpn name changed
//    if (vpnList.size() == m_vpnGroup->itemCount())
//        return;

    m_vpnGroup->clear();
    qDeleteAll(m_vpns.keys());
    m_vpns.clear();

    for (const auto &vpn : vpnList)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(vpn.value("Id").toString());

        connect(w, &NextPageWidget::acceptNextPage, this, &VpnPage::onVpnDetailClicked);
        connect(w, &NextPageWidget::selected, this, &VpnPage::onVpnSelected);

        m_vpns[w] = vpn;
        m_vpnGroup->appendItem(w);
    }

    onActiveConnsInfoChanged(m_model->activeConnInfos());
}

void VpnPage::onVpnDetailClicked()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_vpns.contains(w));

    m_editingConnUuid = m_vpns[w].value("Uuid").toString();

    emit requestEditVpn("/", m_editingConnUuid);
}

void VpnPage::onVpnSelected()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_vpns.contains(w));

    const auto info = m_vpns[w];
    const QString uuid = info.value("Uuid").toString();

    emit requestActivateConnection("/", uuid);
}

void VpnPage::onSessionPageFinished()
{
    if (m_vpnTypePage)
        emit m_vpnTypePage->back();
}

void VpnPage::onVpnSessionCreated(const QString &device, const QString &sessionPath)
{
    Q_ASSERT(device == "/");
    Q_ASSERT(m_editPage.isNull());

    m_editPage = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_editPage);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(sessionPath, sessionModel, m_editPage);

    m_editPage->setModel(m_model, sessionModel);
    connect(m_editPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(m_editPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(m_editPage, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::back, this, &VpnPage::onSessionPageFinished, Qt::QueuedConnection);
    connect(m_editPage, &ConnectionEditPage::requestRemove, [=] { emit requestDeleteConnection(m_editingConnUuid); });
    connect(m_editPage, &ConnectionEditPage::requestDisconnect, [=] { emit requestDeactiveConnection(m_editingConnUuid); });
    connect(m_editPage, &ConnectionEditPage::requestFrameKeepAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    emit requestNextPage(m_editPage);
}

void VpnPage::onActiveConnsInfoChanged(const QList<QJsonObject> &infos)
{
    QList<QString> activeVpnTitles;

    for (const auto &info : infos)
    {
        const QString type = info.value("ConnectionType").toString();
        if (!type.startsWith("vpn"))
            continue;

        const QString name = info.value("ConnectionName").toString();
        activeVpnTitles << name;
    }

    for (auto it(m_vpns.cbegin()); it != m_vpns.cend(); ++it)
    {
        const QString t = it.key()->title();

        if (activeVpnTitles.contains(t))
            it.key()->setIcon(QPixmap(":/network/themes/dark/icons/select.png"));
        else
            it.key()->clearValue();
    }
}

void VpnPage::createVPNSession()
{
//    emit requestCreateConnection("vpn", "/");

    if (!m_vpnTypePage)
    {
        OptionItem *l2tp = new OptionItem;
        l2tp->setContentsMargins(20, 0, 10, 0);
        l2tp->setTitle(tr("L2TP"));
        OptionItem *pptp = new OptionItem;
        pptp->setContentsMargins(20, 0, 10, 0);
        pptp->setTitle(tr("PPTP"));
        OptionItem *vpnc = new OptionItem;
        vpnc->setContentsMargins(20, 0, 10, 0);
        vpnc->setTitle(tr("VPNC"));
        OptionItem *openvpn = new OptionItem;
        openvpn->setContentsMargins(20, 0, 10, 0);
        openvpn->setTitle(tr("OpenVPN"));
        OptionItem *strongswan = new OptionItem;
        strongswan->setContentsMargins(20, 0, 10, 0);
        strongswan->setTitle(tr("StrongSwan"));
        OptionItem *openconnect = new OptionItem;
        openconnect->setContentsMargins(20, 0, 10, 0);
        openconnect->setTitle(tr("OpenConnect"));

        connect(l2tp, &OptionItem::selectedChanged, [=] { createVPN("vpn-l2tp"); });
        connect(pptp, &OptionItem::selectedChanged, [=] { createVPN("vpn-pptp"); });
        connect(vpnc, &OptionItem::selectedChanged, [=] { createVPN("vpn-vpnc"); });
        connect(openvpn, &OptionItem::selectedChanged, [=] { createVPN("vpn-openvpn"); });
        connect(strongswan, &OptionItem::selectedChanged, [=] { createVPN("vpn-strongswan"); });
        connect(openconnect, &OptionItem::selectedChanged, [=] { createVPN("vpn-openconnect"); });

        TranslucentFrame *widget = new TranslucentFrame;
        QVBoxLayout *mainLayout = new QVBoxLayout(widget);
        mainLayout->setMargin(0);

        SettingsGroup *grp = new SettingsGroup;
        grp->appendItem(l2tp);
        grp->appendItem(pptp);
        grp->appendItem(vpnc);
        grp->appendItem(openvpn);
        grp->appendItem(strongswan);
        grp->appendItem(openconnect);

        mainLayout->addSpacing(10);
        mainLayout->addWidget(grp);

        m_vpnTypePage = new ContentWidget;
        m_vpnTypePage->setTitle(tr("New VPN"));
        m_vpnTypePage->setContent(widget);

        connect(m_vpnTypePage, &ContentWidget::back, [=] { m_vpnTypePage = nullptr; });
    }

    emit requestNextPage(m_vpnTypePage);
}

void VpnPage::createVPN(const QString &type)
{
    Q_ASSERT(m_vpnTypePage);

    emit requestCreateConnection(type, "/");
}
