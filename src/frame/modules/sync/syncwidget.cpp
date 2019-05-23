#include "syncwidget.h"
#include "syncmodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include <QDebug>

using namespace dcc;
using namespace dcc::sync;
using namespace dcc::widgets;

SyncWidget::SyncWidget()
    : ModuleWidget()
    , m_accountBtn(new QPushButton(tr("Login")))
    , m_userButton(new NextPageWidget)
{
    setObjectName("deepin ID");
    setTitle(tr("deepin ID"));

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    m_userButtonGrp = new SettingsGroup;
    m_userButtonGrp->appendItem(m_userButton);

    m_centralLayout->addWidget(m_userButtonGrp);
    m_centralLayout->addWidget(m_accountBtn);

    m_userButtonGrp->hide();
    m_userButton->hide();

    connect(m_accountBtn, &QPushButton::clicked, this, &SyncWidget::requestLogin);
    connect(m_userButton, &NextPageWidget::clicked, this, &SyncWidget::requestShowSyncDetails);
}

void SyncWidget::setModel(const SyncModel * const model)
{
    m_model = model;

    connect(model, &SyncModel::userInfoChanged, this, &SyncWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void SyncWidget::onUserInfoChanged(const QVariantMap &userinfo)
{
    const QString& username = userinfo["Username"].toString();
    const bool isLogind = !username.isEmpty();

    m_accountBtn->setVisible(!isLogind);
    m_userButton->setVisible(isLogind);
    m_userButtonGrp->setVisible(isLogind);

    if (isLogind) {
        m_userButton->setTitle(username);
    }
}
