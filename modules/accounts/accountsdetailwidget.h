#ifndef ACCOUNTSDETAILWIDGET_H
#define ACCOUNTSDETAILWIDGET_H

#include "contentwidget.h"
#include "user.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

class AccountsDetailWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(User *user, QWidget *parent = 0);

signals:
    void requestSetAutoLogin(User *user, const bool autoLogin) const;

private:
    dcc::SettingsGroup *m_accountSettings;
    dcc::NextPageWidget *m_modifyAvatar;
    dcc::NextPageWidget *m_modifyPassword;
    dcc::SwitchWidget *m_autoLogin;
};

#endif // ACCOUNTSDETAILWIDGET_H
