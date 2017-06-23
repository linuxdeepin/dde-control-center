#include "pppoepage.h"
#include "settingsgroup.h"
#include "networkmodel.h"
#include "wireddevice.h"
#include "settingsheaderitem.h"
#include "translucentframe.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"
#include "connectioneditpage.h"

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent),
      m_settingsGrp(new SettingsGroup),
      m_createBtn(new QPushButton)
{
    m_createBtn->setText(tr("Create PPPoE Connection"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_settingsGrp);
    mainLayout->addWidget(m_createBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("PPP"));

    connect(m_createBtn, &QPushButton::clicked, this, &PppoePage::createPPPoEConnection);
}

PppoePage::~PppoePage()
{
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::unhandledConnectionSessionCreated, this, &PppoePage::onConnectionSessionCreated);
    connect(model, &NetworkModel::connectionListChanged, this, &PppoePage::onConnectionListChanged);
    connect(model, &NetworkModel::activeConnectionsChanged, this, &PppoePage::onActivateConnectionChanged);

    onConnectionListChanged();
}

void PppoePage::createPPPoEConnection()
{
    emit requestCreateConnection("pppoe", "/");
}

void PppoePage::onConnectionListChanged()
{
    m_settingsGrp->clear();
    qDeleteAll(m_connUuid.keys());
    m_connUuid.clear();

    for (const auto &pppoe : m_model->pppoes())
    {
        const auto name = pppoe.value("Id").toString();
        const auto uuid = pppoe.value("Uuid").toString();

        NextPageWidget *w = new NextPageWidget;
        w->setTitle(name);

        connect(w, &NextPageWidget::acceptNextPage,  this, &PppoePage::onConnectionDetailClicked);
        connect(w, &NextPageWidget::selected, this, &PppoePage::onPPPoESelected);

        m_settingsGrp->appendItem(w);
        m_connUuid[w] = uuid;
    }

    onActivateConnectionChanged(m_model->activeConnections());
}

void PppoePage::onConnectionDetailClicked()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_connUuid.contains(w));

    m_editingUuid = m_connUuid[w];

    emit requestEditConnection("/", m_editingUuid);
}

void PppoePage::onConnectionSessionCreated(const QString &devicePath, const QString &sessionPath)
{
    Q_ASSERT(devicePath == "/");
    Q_ASSERT(m_editPage.isNull());

    m_editPage = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_editPage);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(sessionPath, sessionModel, m_editPage);

    m_editPage->setModel(m_model, sessionModel);
    connect(m_editPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(m_editPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(m_editPage, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestRemove, [this] { emit requestDeleteConnection(m_editingUuid); });
    connect(m_editPage, &ConnectionEditPage::requestDisconnect, [this] { emit requestDisconnectConnection(m_editingUuid); });
    connect(m_editPage, &ConnectionEditPage::requestFrameKeepAutoHide, this, &PppoePage::requestFrameKeepAutoHide);
    emit requestNextPage(m_editPage);
}

void PppoePage::onPPPoESelected()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_connUuid.contains(w));

    m_editingUuid = m_connUuid[w];
    emit requestActivateConnection("/", m_editingUuid);
}

void PppoePage::onActivateConnectionChanged(const QSet<QString> &conns)
{
    for (NextPageWidget *widget : m_connUuid.keys())
        widget->setIcon(QPixmap());

    for (const QString &uuid : conns) {
        NextPageWidget *w = m_connUuid.key(uuid);
        if (w) {
            w->setIcon(QPixmap(":/network/themes/dark/icons/select.png"));
            return;
        }
    }
}
