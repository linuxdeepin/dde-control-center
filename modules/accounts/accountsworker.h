#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_accountsuser.h>

#include "user.h"
#include "usermodel.h"

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

    UserModel * m_userList;
};

#endif // ACCOUNTSWORKER_H
