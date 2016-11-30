#include "accountsdetailwidget.h"
#include "nextpagewidget.h"
#include "translucentframe.h"

#include <QVBoxLayout>

using namespace dcc;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_accountSettings(new SettingsGroup),
      m_modifyAvatar(new NextPageWidget),
      m_modifyPassword(new NextPageWidget),
      m_autoLogin(new SwitchWidget),
      m_deleteAccount(new QPushButton)
{
    m_modifyAvatar->setTitle(tr("Modify Avatar"));
    m_accountSettings->appendItem(m_modifyAvatar);

    m_modifyPassword->setTitle(tr("Modify Password"));
    m_accountSettings->appendItem(m_modifyPassword);

    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(user->autoLogin());
    m_accountSettings->appendItem(m_autoLogin);

    m_deleteAccount->setText(tr("Delete Account"));
    m_deleteAccount->setObjectName("DeleteAccountButton");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_accountSettings);
    mainLayout->addWidget(m_deleteAccount);
    mainLayout->setMargin(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    connect(user, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_deleteAccount, &QPushButton::clicked, [=] { emit requestDeleteAccount(user); });
    connect(m_autoLogin, &SwitchWidget::checkedChanegd, [=] (const bool autoLogin) { emit requestSetAutoLogin(user, autoLogin); });
    connect(m_modifyPassword, &NextPageWidget::clicked, [=] { emit showPwdSettings(user); });
    connect(m_modifyAvatar, &NextPageWidget::clicked, [=] { emit showAvatarSettings(user); });

    setContent(mainWidget);
    setTitle(user->name());
}
