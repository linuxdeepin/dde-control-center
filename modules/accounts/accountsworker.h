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

public slots:
    void onUserListChanged(const QStringList &userList);

private:
    Accounts *m_accountsInter;
    QMap<AccountsUser *, QString> m_userInters;

    UserModel *m_userList;
};

#endif // ACCOUNTSWORKER_H
