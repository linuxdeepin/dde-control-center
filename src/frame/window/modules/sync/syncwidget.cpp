#include "syncwidget.h"

#include "pages/login.h"
#include "pages/index.h"

#include "modules/sync/syncmodel.h"

#include <QVBoxLayout>
#include <QStackedLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncWidget::SyncWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QStackedLayout)
    , m_model(nullptr)
    , m_loginPage(new LoginPage)
    , m_indexPage(new IndexPage)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_mainLayout->addWidget(m_loginPage);
    m_mainLayout->addWidget(m_indexPage);

    setLayout(m_mainLayout);

    connect(m_loginPage, &LoginPage::requestLoginUser, this, &SyncWidget::requestLoginUser);
}

void SyncWidget::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;

    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &SyncWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void SyncWidget::onUserInfoChanged(const QVariantMap &userInfo)
{
    const bool isLogind = !userInfo["Username"].toString().isEmpty();

    if (isLogind) {
        m_mainLayout->setCurrentWidget(m_indexPage);
    }
    else {
        m_mainLayout->setCurrentWidget(m_loginPage);
    }
}
