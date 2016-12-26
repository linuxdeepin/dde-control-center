#include "pppoepage.h"
#include "settingsgroup.h"
#include "networkmodel.h"
#include "wireddevice.h"
#include "settingsheaderitem.h"
#include "translucentframe.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "connectioneditpage.h"

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent),
      m_settingsGrp(new SettingsGroup),
      m_pppoeSwitch(new SwitchWidget),
      m_createBtn(new QPushButton)
{
    m_settingsGrp->appendItem(m_pppoeSwitch);
    m_createBtn->setText(tr("Create PPPoE Connection"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_settingsGrp);
    mainLayout->addWidget(m_createBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

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

    for (const auto &pppoe : m_model->pppoes())
    {
        const auto name = pppoe.value("Id").toString();
        const auto path = pppoe.value("Path").toString();

        NextPageWidget *w = new NextPageWidget;
        w->setTitle(name);

        connect(w, &NextPageWidget::clicked,  this, &PppoePage::onConnectionDetailClicked);

        m_settingsGrp->appendItem(w);
        m_connPath[w] = path;
    }
}

void PppoePage::createPPPoEConnection()
{
    emit requestCreateConnection("pppoe", "/");
}

void PppoePage::onConnectionDetailClicked()
{
    NextPageWidget *w = static_cast<NextPageWidget *>(sender());
    Q_ASSERT(w && m_connPath.contains(w));

    emit requestEditConnection("/", m_connPath[w]);
}

void PppoePage::onConnectionSessionCreated(const QString &devicePath, const QString &sessionPath)
{
    Q_ASSERT(devicePath == "/");

    qDebug() << sessionPath;
}
