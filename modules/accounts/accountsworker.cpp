#include "accountsworker.h"

#include <QFileDialog>
#include <QtConcurrent>
#include <QFutureWatcher>

#include "user.h"

const QString AccountsService("com.deepin.daemon.Accounts");

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent),
      m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this)),
      m_userModel(userList)
{
    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged);
    connect(m_accountsInter, &Accounts::UserAdded, this, &AccountsWorker::addUser);
    connect(m_accountsInter, &Accounts::UserDeleted, this, &AccountsWorker::removeUser);

    onUserListChanged(m_accountsInter->userList());

    m_accountsInter->setSync(false);
}

void AccountsWorker::randomUserIcon(User *user)
{
    QDBusPendingCall call = m_accountsInter->RandUserIcon();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [user, call] {
        if (!call.isError()) {
            QDBusReply<QString> reply = call.reply();
            user->setCurrentAvatar(reply.value());
        }
    });
}

void AccountsWorker::createAccount(const User *user)
{
    qDebug() << "create account " << user;

    QFutureWatcher<CreationResult*> *watcher = new QFutureWatcher<CreationResult*>(this);
    connect(watcher, &QFutureWatcher<CreationResult*>::finished, [this, watcher] {
        CreationResult *result = watcher->result();
        emit accountCreationFinished(result);
    });

    QFuture<CreationResult*> future = QtConcurrent::run(this, &AccountsWorker::createAccountInternal, user);
    watcher->setFuture(future);
}

void AccountsWorker::addNewAvatar(User *user)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    emit requestFrameAutoHide(false);
    QFileDialog fd;
    do {
        if (fd.exec() != QFileDialog::Accepted)
            break;

        const QString file = fd.selectedFiles().first();
        userInter->SetIconFile(file).waitForFinished();
    } while (false);

    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
}

void AccountsWorker::setAvatar(User *user, const QString &iconPath)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    ui->SetIconFile(iconPath);
}

void AccountsWorker::deleteAccount(User *user, const bool deleteHome)
{
    m_accountsInter->DeleteUser(user->name(), deleteHome);
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
    if (m_userInters.size() == 0) {
        const QSet<QString> userSet = userList.toSet();
        for (const QString &userPath : userSet)
        {
            addUser(userPath);
        }
    }
}

void AccountsWorker::setPassword(User *user, const QString &passwd)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    emit requestFrameAutoHide(false);
    userInter->SetPassword(passwd).waitForFinished();
    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
}

void AccountsWorker::addUser(const QString &userPath)
{
    qDebug() << "user added: " << userPath;
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

void AccountsWorker::removeUser(const QString &userPath)
{
    qDebug() << "user removed: " << userPath;
    for (AccountsUser *userInter : m_userInters.values()) {
        if (userInter->path() == userPath) {
            User *user = m_userInters.key(userInter);
            m_userInters.remove(user);

            m_userModel->removeUser(userPath);
        }
    }
}

CreationResult *AccountsWorker::createAccountInternal(const User *user)
{
    CreationResult *result = new CreationResult;

    // validate username
    QDBusPendingReply<bool, QString, int> reply = m_accountsInter->IsUsernameValid(user->name());
    reply.waitForFinished();
    if (reply.isError()) {
        result->setType(CreationResult::UserNameError);
        result->setMessage(reply.error().message());
        return result;
    }
    bool validation = reply.argumentAt(0).toBool();
    if (!validation) {
        result->setType(CreationResult::UserNameError);
        result->setMessage(reply.argumentAt(1).toString());
        return result;
    }

    // validate password
    if (user->password() != user->repeatPassword()) {
        result->setType(CreationResult::PasswordMatchError);
        result->setMessage(tr("Password not match."));
        return result;
    }

    QDBusPendingReply<> reply1 = m_accountsInter->CreateUser(user->name(), user->name(), 1);
    reply1.waitForFinished();
    if (reply1.isError()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage(reply1.error().message());
        return result;
    }

    QDBusPendingReply<QString> reply2 = m_accountsInter->FindUserByName(user->name());
    reply2.waitForFinished();
    if (reply2.isError()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage(reply2.error().message());
        return result;
    }

    QString userPath = reply2.argumentAt(0).toString();
    AccountsUser *userDBus = new AccountsUser("com.deepin.daemon.Accounts", userPath, QDBusConnection::systemBus(), this);
    if (!userDBus->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    //TODO(hualet): better to check all the call results.
    bool sifResult = userDBus->SetIconFile(user->currentAvatar());
    bool spResult = userDBus->SetPassword(user->password());

    if (!sifResult || !spResult) {
        result->setType(CreationResult::UnknownError);
        if (!sifResult) result->setMessage("set icon file for new created user failed.");
        if (!spResult) result->setMessage("set password for new created user failed");

        return result;
    }

    return result;
}
