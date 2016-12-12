#ifndef ACCOUNTSDETAILWIDGET_H
#define ACCOUNTSDETAILWIDGET_H

#include "contentwidget.h"
#include "user.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>

namespace dcc {
namespace accounts {

class AccountsDetailWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(User *user, QWidget *parent = 0);

signals:
    void showPwdSettings(User *user) const;
    void showAvatarSettings(User *user) const;

    void requestDeleteAccount(User *user, const bool deleteHome) const;
    void requestSetAutoLogin(User *user, const bool autoLogin) const;

    void requestChangeFrameAutoHide(const bool autoHide) const;

private slots:
    void deleteUserClicked();

private:
    User *m_user;
    dcc::widgets::SettingsGroup *m_accountSettings;
    dcc::widgets::NextPageWidget *m_modifyAvatar;
    dcc::widgets::NextPageWidget *m_modifyPassword;
    dcc::widgets::SwitchWidget *m_autoLogin;
    QPushButton *m_deleteAccount;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSDETAILWIDGET_H
