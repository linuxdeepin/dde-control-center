#include "syncwidget.h"

#include "pages/login.h"
#include "pages/index.h"
#include "pages/logout.h"

#include "modules/sync/syncmodel.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncWidget::SyncWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QStackedLayout(this))
    , m_model(nullptr)
    , m_loginPage(new LoginPage)
    , m_indexPage(new IndexPage)
    , m_cnonlyPage(new LogoutPage)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_indexPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mainLayout->addWidget(m_loginPage);
    m_mainLayout->addWidget(m_indexPage);
    m_mainLayout->addWidget(m_cnonlyPage);
    setLayout(m_mainLayout);

    connect(m_loginPage, &LoginPage::requestLoginUser, this, &SyncWidget::requestLoginUser);
    connect(m_indexPage, &IndexPage::requestSetAutoSync, this, &SyncWidget::requestSetAutoSync);
    connect(m_indexPage, &IndexPage::requestLogout, this, &SyncWidget::requestLogoutUser);
    connect(m_indexPage, &IndexPage::requestSetModuleState, this, &SyncWidget::requestSetModuleState);
    connect(m_cnonlyPage, &LogoutPage::requestLogout, this, &SyncWidget::requestLogoutUser);
}

void SyncWidget::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;
    m_indexPage->setModel(model);
    m_cnonlyPage->setModel(model);

    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &SyncWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void SyncWidget::onUserInfoChanged(const QVariantMap &userInfo)
{
    const bool isLogind = !userInfo["Username"].toString().isEmpty();
    const QString region = userInfo["Region"].toString();

    if (isLogind) {
        if (region == "CN") {
            m_mainLayout->setCurrentWidget(m_indexPage);
        } else {
            m_mainLayout->setCurrentWidget(m_cnonlyPage);
        }
    }
    else {
        m_mainLayout->setCurrentWidget(m_loginPage);
    }
}
