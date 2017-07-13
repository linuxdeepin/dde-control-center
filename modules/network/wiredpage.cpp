#include "wiredpage.h"
#include "wireddevice.h"
#include "networkmodel.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"
#include "connectionsessionmodel.h"
#include "connectionsessionworker.h"
#include "tipsitem.h"

#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointer>

namespace dcc {

namespace network {

using namespace widgets;

WiredPage::WiredPage(WiredDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev)
{
    m_settingsGrp = new SettingsGroup;
    m_settingsGrp->setHeaderVisible(true);
    m_settingsGrp->headerItem()->setTitle(tr("Setting List"));

    TipsItem *tips = new TipsItem;
    tips->setFixedHeight(80);
    tips->setText(tr("Please firstly plug in the network cable"));

    m_tipsGrp = new SettingsGroup;
    m_tipsGrp->appendItem(tips);

    m_createBtn = new QPushButton;
    m_createBtn->setText(tr("Add Settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addSpacing(10);
    centralLayout->addWidget(m_settingsGrp);
    centralLayout->addWidget(m_tipsGrp);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();
    centralLayout->setSpacing(10);
    centralLayout->setMargin(0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Select Settings"));

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::sessionCreated, this, &WiredPage::onSessionCreated);
    connect(m_device, &WiredDevice::connectionsChanged, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::activeConnectionChanged, this, &WiredPage::checkActivatedConnection);
    connect(m_device, &WiredDevice::removed, this, &WiredPage::onDeviceRemoved);
    connect(m_device, static_cast<void (WiredDevice::*)(WiredDevice::DeviceStatus) const>(&WiredDevice::statusChanged), this, &WiredPage::onDeviceStatusChanged);

    onDeviceStatusChanged(m_device->status());
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
    emit requestConnectionsList(m_device->path());
}

void WiredPage::refreshConnectionList()
{
    // get all available wired connections path
    const auto wiredConns = m_model->wireds();

    QSet<QString> availableWiredConns;
    availableWiredConns.reserve(wiredConns.size());

    for (const auto &wiredConn : wiredConns)
    {
        const QString path = wiredConn.value("Path").toString();
        if (!path.isEmpty())
            availableWiredConns << path;
    }

    const auto conns = m_device->connections();
    QSet<QString> connPaths;
    for (const auto c : conns)
    {
        const QString path = c.path();
        // pass unavailable wired conns, like 'PPPoE'
        if (!availableWiredConns.contains(path))
            continue;

        connPaths << path;
        if (m_connectionPath.values().contains(path))
            continue;

        NextPageWidget *w = new NextPageWidget;
        w->setTitle(m_model->connectionNameByPath(path));

        connect(w, &NextPageWidget::acceptNextPage, this, &WiredPage::editConnection);
        connect(w, &NextPageWidget::selected, this, &WiredPage::activeConnection);

        m_settingsGrp->appendItem(w);
        m_connectionPath.insert(w, path);
    }

    // clear removed items
    for (auto it(m_connectionPath.begin()); it != m_connectionPath.end();)
    {
        if (!connPaths.contains(it.value()))
        {
            it.key()->deleteLater();
            it = m_connectionPath.erase(it);
        } else {
            ++it;
        }
    }

    checkActivatedConnection();
}

void WiredPage::editConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString connPath = m_connectionPath[w];

    emit requestEditConnection(m_device->path(), m_model->connectionUuidByPath(connPath));
}

void WiredPage::createNewConnection()
{
    emit requestCreateConnection("wired", m_device->path());
}

void WiredPage::activeConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString connPath = m_connectionPath[w];

    emit requestActiveConnection(m_device->path(), m_model->connectionUuidByPath(connPath));
}

void WiredPage::checkActivatedConnection()
{
    static QPixmap selected_icon(":/network/themes/dark/icons/select.png");

    const auto activeConnection = m_device->activeConnection().value("ConnectionName").toString();
    for (auto it(m_connectionPath.cbegin()); it != m_connectionPath.cend(); ++it)
    {
        if (it.key()->title() == activeConnection)
            it.key()->setIcon(selected_icon);
        else
            it.key()->clearValue();
    }
}

void WiredPage::onDeviceStatusChanged(const NetworkDevice::DeviceStatus stat)
{
    // FIXME: when device not ready, ListDeviceConncections of deepin-daemon will return a empty list,
    // we need to refetch data when device is ready.
    emit requestConnectionsList(m_device->path());

    const bool unavailable = stat <= NetworkDevice::Unavailable;

    m_tipsGrp->setVisible(unavailable);
    m_settingsGrp->setVisible(!unavailable);
    m_createBtn->setVisible(!unavailable);
}

void WiredPage::onSessionCreated(const QString &sessionPath)
{
    Q_ASSERT(m_editPage.isNull());

    m_editPage = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_editPage);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(sessionPath, sessionModel, m_editPage);

    m_editPage->setModel(m_model, sessionModel);
    connect(m_editPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(m_editPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(m_editPage, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestRemove, this, &WiredPage::requestDeleteConnection);
    connect(m_editPage, &ConnectionEditPage::requestDisconnect, this, &WiredPage::requestDisconnectConnection);

    emit requestNextPage(m_editPage);
}

void WiredPage::onDeviceRemoved()
{
    if (!m_editPage.isNull())
        emit m_editPage->back();

    emit back();
}

}

}
