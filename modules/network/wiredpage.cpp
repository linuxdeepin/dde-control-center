#include "wiredpage.h"
#include "wireddevice.h"
#include "networkmodel.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"
#include "connectionsessionmodel.h"
#include "connectionsessionworker.h"

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
    m_settingsGrp->headerItem()->setTitle("Settings List");

    m_createBtn = new QPushButton;
    m_createBtn->setText(tr("New Settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addSpacing(10);
    centralLayout->addWidget(m_settingsGrp);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();
    centralLayout->setSpacing(10);
    centralLayout->setMargin(0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Choose Settings"));

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::sessionCreated, this, &WiredPage::onSessionCreated);
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
        const QString path = c.path();

        NextPageWidget *w = new NextPageWidget;
        w->setTitle(m_model->connectionNameByPath(path));

        connect(w, &NextPageWidget::acceptNextPage, this, &WiredPage::editConnection);
        connect(w, &NextPageWidget::selected, this, &WiredPage::activeConnection);

        m_settingsGrp->appendItem(w);
        m_connectionUuid.insert(w, m_model->connectionUuidByPath(path));
    }
}

void WiredPage::editConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString uuid = m_connectionUuid[w];

    emit requestEditConnection(m_device->path(), uuid);
}

void WiredPage::createNewConnection()
{
    emit requestCreateConnection("wired", m_device->path());
}

void WiredPage::activeConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString uuid = m_connectionUuid[w];

    emit requestActiveConnection(m_device->path(), uuid);
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
//    connect(m_editPage, &ConnectionEditPage::requestRemove, [this] { emit requestDeleteConnection(m_editingUuid); });
//    connect(m_editPage, &ConnectionEditPage::requestDisconnect, [this] { emit requestDisconnectConnection(m_editingUuid); });
//    connect(m_editPage, &ConnectionEditPage::requestFrameKeepAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    emit requestNextPage(m_editPage);
}

}

}
