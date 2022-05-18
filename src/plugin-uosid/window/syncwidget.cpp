#include "syncwidget.h"
#include "syncmodel.h"
#include "widgets/login.h"
#include "widgets/logininfo.h"
#include "widgets/logininfodetail.h"
#include "widgets/avatarwidget.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <DFloatingButton>
#include <DBackgroundGroup>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

/**
 * @brief SyncWidget::SyncWidget  UnionID 二级页面
 */

SyncWidget::SyncWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QStackedLayout)
    , m_model(nullptr)
    , m_loginPage(new LoginPage)
    , m_loginInfoPage(new LoginInfoPage)
    , m_loginInfoDetailPage(new LoginInfoDetailPage)
    , m_groundGroup(new DBackgroundGroup)
    , m_isLogind(false)
{
    initUI();
    initConnect();
}

SyncWidget::~SyncWidget()
{

}

void SyncWidget::setModel(SyncModel *model)
{
    m_model = model;
    m_loginInfoPage->setModel(model);
    m_loginInfoDetailPage->setModel(model);

    connect(model, &SyncModel::userInfoChanged, this, &SyncWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void SyncWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

void SyncWidget::initUI()
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    QHBoxLayout *contentLay = new QHBoxLayout;
    contentLay->setContentsMargins(0, 0, 0, 0);
    contentLay->setSpacing(0);
    m_groundGroup->setLayout(contentLay);
    m_groundGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_loginInfoDetailPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_groundGroup->setContentsMargins(0, 0, 0, 0);
    m_groundGroup->setItemSpacing(2);
    m_groundGroup->setBackgroundRole(QPalette::Base);
    m_groundGroup->setUseWidgetBackground(true);

    // TODO: 暂时设置固定值
    m_loginInfoPage->setFixedWidth(220);
    contentLay->addWidget(m_loginInfoPage, 1);
    contentLay->addWidget(m_loginInfoDetailPage, 2);

    m_mainLayout->addWidget(m_loginPage);
    m_mainLayout->addWidget(m_groundGroup);
    setLayout(m_mainLayout);
}

void SyncWidget::initConnect()
{
    connect(m_loginPage, &LoginPage::requestLoginUser, this, &SyncWidget::requestLoginUser);
    connect(m_loginInfoPage, &LoginInfoPage::requestLogoutUser, this, &SyncWidget::requestAsyncLogoutUser);
    connect(m_loginInfoPage, &LoginInfoPage::requestSetFullname, this, &SyncWidget::requestSetFullname);
    connect(m_loginInfoPage, &LoginInfoPage::requestPullMessage, this, &SyncWidget::requestPullMessage);

    connect(m_loginInfoDetailPage, &LoginInfoDetailPage::requestBindAccount, this, &SyncWidget::requestBindAccount);
    connect(m_loginInfoDetailPage, &LoginInfoDetailPage::requestUnBindAccount, this, &SyncWidget::requestUnBindAccount);
    connect(m_loginInfoDetailPage, &LoginInfoDetailPage::requestSetAutoSync, this, &SyncWidget::requestSetAutoSync);
    connect(m_loginInfoDetailPage, &LoginInfoDetailPage::requestSetModuleState, this, &SyncWidget::requestSetModuleState);
    connect(m_loginInfoDetailPage, &LoginInfoDetailPage::requestLocalBindCheck, this, &SyncWidget::requestLocalBindCheck);
}

void SyncWidget::onUserInfoChanged(const QVariantMap &userInfo)
{
    bool isLogind = !userInfo["Username"].toString().isEmpty();
    if (isLogind) {
        m_mainLayout->setCurrentWidget(m_groundGroup);
        setBackgroundRole(QPalette::Window);
        setAutoFillBackground(true);
    } else {
        m_mainLayout->setCurrentWidget(m_loginPage);
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
    }
}


