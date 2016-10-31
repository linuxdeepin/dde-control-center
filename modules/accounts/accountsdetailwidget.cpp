#include "accountsdetailwidget.h"

#include <nextpagewidget.h>

using namespace dcc;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_accountSettings(new SettingsGroup),
      m_modifyAvatar(new NextPageWidget),
      m_modifyPassword(new NextPageWidget),
      m_autoLogin(new SwitchWidget)
{
    m_modifyAvatar->setTitle(tr("Modify Avatar"));
    m_accountSettings->appendItem(m_modifyAvatar);

    m_modifyPassword->setTitle(tr("Modify Password"));
    m_accountSettings->appendItem(m_modifyPassword);

    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(user->autoLogin());
    m_accountSettings->appendItem(m_autoLogin);

    connect(user, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_autoLogin, &SwitchWidget::checkedChanegd, [=] (const bool autoLogin) { emit requestSetAutoLogin(user, autoLogin); });

    setContent(m_accountSettings);
    setTitle(user->name());
}
