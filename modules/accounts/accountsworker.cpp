#include "accountsworker.h"

const QString AccountsService("com.deepin.daemon.Accounts");

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent),
      m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this)),
      m_userModel(userList)
{
    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged);

    onUserListChanged(m_accountsInter->userList());

    m_accountsInter->setSync(false);
}

void AccountsWorker::createAccount()
{
    qDebug() << Q_FUNC_INFO;
}

void AccountsWorker::deleteAccount(User *user)
{
    qDebug() << "delete user " << user->name();
    qDebug() << Q_FUNC_INFO;
}

void AccountsWorker::setAutoLogin(User *user, const bool autoLogin)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    // because this operate need root permission, we must wait for finished and refersh result
    emit requestFrameAutoHide(false);
    ui->SetAutomaticLogin(autoLogin).waitForFinished();
    user->setAutoLogin(ui->automaticLogin());
    // delay 100ms because click event is coming after we reset auto hide to `true`
    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
}

void AccountsWorker::onUserListChanged(const QStringList &userList)
{
    const QSet<QString> userSet = userList.toSet();
    for (const QString &userPath : userSet)
    {
        if (m_userSet.contains(userPath))
            continue;

        AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);

        User * user = new User(m_userModel);
        connect(userInter, &AccountsUser::UserNameChanged, user, &User::setName);
        connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);
        connect(userInter, &AccountsUser::IconListChanged, user, &User::setAvatars);
        connect(userInter, &AccountsUser::IconFileChanged, user, &User::setCurrentAvatar);

        user->setName(userInter->userName());
        user->setAutoLogin(userInter->automaticLogin());
        user->setAvatars(userInter->iconList());
        user->setCurrentAvatar(userInter->iconFile());

        userInter->setSync(false);

        m_userInters[user] = userInter;
        m_userModel->addUser(userPath, user);
    }

    // TODO: process removed user
    m_userSet = userSet;
}

void AccountsWorker::setPassword(User *user, const QString &passwd)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    emit requestFrameAutoHide(false);
    userInter->SetPassword(passwd).waitForFinished();
    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
}
