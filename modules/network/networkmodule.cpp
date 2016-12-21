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

    return m_networkWidget;
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev)
{
    ContentWidget *c = nullptr;

    if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessPage *p = new WirelessPage(static_cast<WirelessDevice *>(dev));
        connect(p, &WirelessPage::requestDeviceStatus, m_networkWorker, &NetworkWorker::queryDeviceStatus);
        connect(p, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(p, &WirelessPage::requestEditAP, m_networkWorker, &NetworkWorker::queryConnectionSession);
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

        connect(dev, &NetworkDevice::sessionCreated, this, &NetworkModule::showWiredConnectionEditPage);
    }

    if (c)
        m_frameProxy->pushWidget(this, c);
}

void NetworkModule::showVpnPage()
{
    VpnPage *p = new VpnPage;

    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);

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

    m_frameProxy->pushWidget(this, p);
}
