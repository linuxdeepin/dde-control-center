#include "networkmodule.h"
#include "networkmodulewidget.h"
#include "contentwidget.h"
#include "modulewidget.h"
#include "networkworker.h"
#include "networkmodel.h"
#include "wirelesspage.h"
#include "wirelessdevice.h"
#include "wireddevice.h"
#include "connectioneditpage.h"
#include "connectionsessionmodel.h"
#include "connectionsessionworker.h"
#include "vpnpage.h"
#include "pppoepage.h"
#include "proxypage.h"
#include "networkdetailpage.h"
#include "wiredpage.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::network;

NetworkModule::NetworkModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_networkModel(nullptr),
      m_networkWorker(nullptr),
      m_networkWidget(nullptr)
{
}

NetworkModule::~NetworkModule()
{
    m_networkModel->deleteLater();
    m_networkWorker->deleteLater();
}

void NetworkModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)

    NetworkDevice *wireless = nullptr;

    for (NetworkDevice *dev : m_networkModel->devices()) {
        if (dev->type() == NetworkDevice::Wireless) {
            if (wireless)
                return;

            wireless = dev;
        }
    }

    if (!wireless)
        return;

    showDeviceDetailPage(wireless);
}

void NetworkModule::initialize()
{
    m_networkModel = new NetworkModel;
    m_networkWorker = new NetworkWorker(m_networkModel);

    m_networkModel->moveToThread(qApp->thread());
    m_networkWorker->moveToThread(qApp->thread());
}

void NetworkModule::reset()
{

}

void NetworkModule::moduleActive()
{

}

void NetworkModule::moduleDeactive()
{

}

void NetworkModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString NetworkModule::name() const
{
    return QStringLiteral("network");
}

ModuleWidget *NetworkModule::moduleWidget()
{
    if (m_networkWidget)
        return m_networkWidget;

    m_networkWidget = new NetworkModuleWidget;
    m_networkWidget->setModel(m_networkModel);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowDeviceDetail, this, &NetworkModule::showDeviceDetailPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowVpnPage, this, &NetworkModule::showVpnPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowPppPage, this, &NetworkModule::showPppPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowProxyPage, this, &NetworkModule::showProxyPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowInfomation, this, &NetworkModule::showDetailPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestDeviceEnable, m_networkWorker, &NetworkWorker::setDeviceEnable);

    return m_networkWidget;
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev)
{
    if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessPage *p = new WirelessPage(static_cast<WirelessDevice *>(dev));
        connect(p, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(p, &WirelessPage::requestEditAP, m_networkWorker, &NetworkWorker::queryConnectionSession);
        connect(p, &WirelessPage::requestConnectAp, m_networkWorker, &NetworkWorker::activateAccessPoint);
        connect(p, &WirelessPage::requestCreateAp, m_networkWorker, &NetworkWorker::createConnection);
        connect(p, &WirelessPage::requestCreateApConfig, m_networkWorker, &NetworkWorker::createApConfig);
        connect(p, &WirelessPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(p, &WirelessPage::requestDeleteConnection, m_networkWorker, &NetworkWorker::deleteConnection);
        connect(p, &WirelessPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
        connect(p, &WirelessPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
        connect(p, &WirelessPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
        p->setModel(m_networkModel);

        m_frameProxy->pushWidget(this, p);
    }
    else if (dev->type() == NetworkDevice::Wired)
    {
        WiredPage *p = new WiredPage(static_cast<WiredDevice *>(dev));
        connect(p, &WiredPage::requestConnectionsList, m_networkWorker, &NetworkWorker::queryDeviceConnections);
        connect(p, &WiredPage::requestActiveConnection, m_networkWorker, &NetworkWorker::activateConnection);
        connect(p, &WiredPage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
        connect(p, &WiredPage::requestEditConnection, m_networkWorker, &NetworkWorker::queryConnectionSession);
        connect(p, &WiredPage::requestDeleteConnection, m_networkWorker, &NetworkWorker::deleteConnection);
        connect(p, &WiredPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
        connect(p, &WiredPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
        p->setModel(m_networkModel);

        m_frameProxy->pushWidget(this, p);
    }
}

void NetworkModule::showVpnPage()
{
    VpnPage *p = new VpnPage;

    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(p, &VpnPage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
    connect(p, &VpnPage::requestDeleteConnection, m_networkWorker, &NetworkWorker::deleteConnection);
    connect(p, &VpnPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &VpnPage::requestDeactiveConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(p, &VpnPage::requestEditVpn, m_networkWorker, &NetworkWorker::queryConnectionSession);
    connect(p, &VpnPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
    connect(p, &VpnPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showPppPage()
{
    PppoePage *p = new PppoePage;

    connect(p, &PppoePage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
    connect(p, &PppoePage::requestEditConnection, m_networkWorker, &NetworkWorker::queryConnectionSession);
    connect(p, &PppoePage::requestDeleteConnection, m_networkWorker, &NetworkWorker::deleteConnection);
    connect(p, &PppoePage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &PppoePage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(p, &PppoePage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
    connect(p, &PppoePage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showProxyPage()
{
    ProxyPage *p = new ProxyPage;

    connect(p, &ProxyPage::requestQueryProxyData, m_networkWorker, &NetworkWorker::queryProxyData);
    connect(p, &ProxyPage::requestSetProxyMethod, m_networkWorker, &NetworkWorker::setProxyMethod);
    connect(p, &ProxyPage::requestSetIgnoreHosts, m_networkWorker, &NetworkWorker::setProxyIgnoreHosts);
    connect(p, &ProxyPage::requestSetProxy, m_networkWorker, &NetworkWorker::setProxy);
    connect(p, &ProxyPage::requestSetAutoProxy, m_networkWorker, &NetworkWorker::setAutoProxy);
    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showDetailPage()
{
    NetworkDetailPage *p = new NetworkDetailPage;

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showWiredConnectionEditPage(const QString &session)
{
    ConnectionEditPage *p = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(p);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(session, sessionModel, p);

    p->setModel(m_networkModel, sessionModel);
    connect(p, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(p, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(p, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(p, &ConnectionEditPage::requestDisconnect, [=] {m_networkWorker->deactiveConnection(m_editingWiredUuid); });
    connect(p, &ConnectionEditPage::requestRemove, [=] {m_networkWorker->deleteConnection(m_editingWiredUuid); });
    connect(p, &ConnectionEditPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
    connect(p, &ConnectionEditPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::onSetFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}
