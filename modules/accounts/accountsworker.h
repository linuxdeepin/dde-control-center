#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>

#include "user.h"
#include "usermodel.h"

using com::deepin::daemon::Accounts;
using AccountsUser = com::deepin::daemon::accounts::User;

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = 0);

signals:
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void setAutoLogin(User *user, const bool autoLogin);
    void onUserListChanged(const QStringList &userList);

private:
    AccountsUser *userInter(const QString &userName) const;

private:
    Accounts *m_accountsInter;
    QSet<QString> m_userSet;
    QMap<User *, AccountsUser *> m_userInters;

    UserModel *m_userModel;
};

#endif // ACCOUNTSWORKER_H
