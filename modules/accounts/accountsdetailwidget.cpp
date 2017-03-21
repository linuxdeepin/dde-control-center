#include "accountsdetailwidget.h"
#include "nextpagewidget.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>

#include "removeuserdialog.h"

using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : ContentWidget(parent),
      m_user(user),

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
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_accountSettings);
    mainLayout->addWidget(m_deleteAccount);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    connect(user, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(user, &User::onlineChanged, this, [this] (const bool online) { m_deleteAccount->setVisible(!online); });
    connect(m_deleteAccount, &QPushButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);
    connect(m_autoLogin, &SwitchWidget::checkedChanged, [=] (const bool autoLogin) { emit requestSetAutoLogin(user, autoLogin); });
    connect(m_modifyPassword, &NextPageWidget::clicked, [=] { emit showPwdSettings(user); });
    connect(m_modifyAvatar, &NextPageWidget::clicked, [=] { emit showAvatarSettings(user); });

    setContent(mainWidget);
    setTitle(user->name());
    m_deleteAccount->setVisible(!user->online());
}

void AccountsDetailWidget::deleteUserClicked()
{
    emit requestChangeFrameAutoHide(false);

    RemoveUserDialog d(m_user);
    int ret = d.exec();

    QTimer::singleShot(300, [this] {
        emit requestChangeFrameAutoHide(true);
    });

    if (ret) {
        emit requestDeleteAccount(m_user, d.deleteHome());
        emit back();
    }
}
