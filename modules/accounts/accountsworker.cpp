#include "accountsworker.h"

const QString AccountsService("com.deepin.daemon.Accounts");

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent),
      m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this)),
      m_userModel(userList)
{
    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged);

    m_accountsInter->setSync(false);
    m_accountsInter->userList();
}

void AccountsWorker::setAutoLogin(User *user, const bool autoLogin)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    // because this operate need root permission, we must wait for finished and refersh result
    ui->SetAutomaticLogin(autoLogin).waitForFinished();
    user->setAutoLogin(ui->automaticLogin());
}

void AccountsWorker::onUserListChanged(const QStringList &userList)
{
    const QSet<QString> userSet = userList.toSet();
    for (const QString &userPath : userSet)
    {
        if (m_userSet.contains(userPath))
            continue;

        AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);
        userInter->setSync(false);

        User * user = new User(m_userModel);
        connect(userInter, &AccountsUser::UserNameChanged, user, &User::setName);
        connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);

        user->setName(userInter->userName());
        user->setAutoLogin(userInter->automaticLogin());

        m_userInters[user] = userInter;
        m_userModel->addUser(userPath, user);
    }

    // TODO: process removed user
    m_userSet = userSet;
}
