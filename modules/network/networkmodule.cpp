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
#include "networkdetailpage.h"

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
    m_networkWidget->deleteLater();
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
    connect(m_networkWidget, &NetworkModuleWidget::requestShowInfomation, this, &NetworkModule::showDetailPage);

    return m_networkWidget;
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev)
{
    ContentWidget *c = nullptr;

    if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessPage *p = new WirelessPage(static_cast<WirelessDevice *>(dev));
        connect(p, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(p, &WirelessPage::requestEditAP, m_networkWorker, &NetworkWorker::queryConnectionSession);
        connect(p, &WirelessPage::requestConnectAp, m_networkWorker, &NetworkWorker::activateAccessPoint);
        connect(p, &WirelessPage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
        connect(p, &WirelessPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(p, &WirelessPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });

        c = p;
    }
    else if (dev->type() == NetworkDevice::Wired)
    {
        const QJsonObject connInfo = static_cast<WiredDevice *>(dev)->connection();
        const QString connPath = connInfo.value("Path").toString();
        const QString devicePath = dev->path();

        m_networkWorker->queryConnectionSession(devicePath, connPath);

        connect(dev, &NetworkDevice::sessionCreated, this, &NetworkModule::showWiredConnectionEditPage, Qt::UniqueConnection);
    }

    if (c)
        m_frameProxy->pushWidget(this, c);
}

void NetworkModule::showVpnPage()
{
    VpnPage *p = new VpnPage;

    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(p, &VpnPage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
    connect(p, &VpnPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &VpnPage::requestEditVpn, m_networkWorker, &NetworkWorker::queryConnectionSession);
    connect(p, &VpnPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showPppPage()
{
    PppoePage *p = new PppoePage;

    connect(p, &PppoePage::requestCreateConnection, m_networkWorker, &NetworkWorker::createConnection);
    connect(p, &PppoePage::requestEditConnection, m_networkWorker, &NetworkWorker::queryConnectionSession);
    connect(p, &PppoePage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });

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

    p->setModel(sessionModel);
    connect(p, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(p, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(p, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(p, &ConnectionEditPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });

    m_frameProxy->pushWidget(this, p);
}
