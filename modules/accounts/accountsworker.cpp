#include "accountsworker.h"
#include "user.h"

#include <QFileDialog>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QStandardPaths>

#include <libintl.h>

using namespace dcc::accounts;

const QString AccountsService("com.deepin.daemon.Accounts");
const QString DisplayManagerService("org.freedesktop.DisplayManager");

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent),
      m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this)),
      m_dmInter(new DisplayManager(DisplayManagerService, "/org/freedesktop/DisplayManager", QDBusConnection::systemBus(), this)),
      m_userModel(userList)
{
    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged);
    connect(m_accountsInter, &Accounts::UserAdded, this, &AccountsWorker::addUser);
    connect(m_accountsInter, &Accounts::UserDeleted, this, &AccountsWorker::removeUser);

    connect(m_dmInter, &DisplayManager::SessionsChanged, this, &AccountsWorker::updateUserOnlineStatus);

    m_accountsInter->setSync(false);
    m_dmInter->setSync(false);

    onUserListChanged(m_accountsInter->userList());
    updateUserOnlineStatus(m_dmInter->sessions());
}

void AccountsWorker::active()
{
    for (auto it(m_userInters.cbegin()); it != m_userInters.cend(); ++it)
    {
        it.key()->setName(it.value()->userName());
        it.key()->setAutoLogin(it.value()->automaticLogin());
        it.key()->setAvatars(it.value()->iconList());
        it.key()->setCurrentAvatar(it.value()->iconFile());
    }
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
    emit requestFrameAutoHide(false);

    QFutureWatcher<CreationResult*> *watcher = new QFutureWatcher<CreationResult*>(this);
    connect(watcher, &QFutureWatcher<CreationResult*>::finished, [this, watcher] {
        CreationResult *result = watcher->result();
        emit accountCreationFinished(result);
        emit requestFrameAutoHide(true);
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
    fd.setNameFilter(tr("Images") + "(*.png *.bmp *.jpg *.jpeg)");

    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    if (!directory.isEmpty())
        fd.setDirectory(directory.first());

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
    emit requestFrameAutoHide(false);
    m_accountsInter->DeleteUser(user->name(), deleteHome).waitForFinished();
    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
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
    for (const auto &path : userList)
        if (!m_userModel->contains(path))
            addUser(path);
}

void AccountsWorker::setPassword(User *user, const QString &passwd)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    emit requestFrameAutoHide(false);
    userInter->SetPassword(passwd).waitForFinished();
    QTimer::singleShot(100, this, [=] { emit requestFrameAutoHide(true); });
}

void AccountsWorker::deleteUserIcon(User *user, const QString &iconPath)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->DeleteIconFile(iconPath);
}

void AccountsWorker::addUser(const QString &userPath)
{
    AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);
    userInter->setSync(false);

    User *user = new User(this);

    connect(userInter, &AccountsUser::UserNameChanged, [=](const QString &name) {
        user->setName(name);
        user->setOnline(m_onlineUsers.contains(name));
    });

    connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);
    connect(userInter, &AccountsUser::IconListChanged, user, &User::setAvatars);
    connect(userInter, &AccountsUser::IconFileChanged, user, &User::setCurrentAvatar);

    user->setName(userInter->userName());
    user->setAutoLogin(userInter->automaticLogin());
    user->setAvatars(userInter->iconList());
    user->setCurrentAvatar(userInter->iconFile());

    m_userInters[user] = userInter;
    m_userModel->addUser(userPath, user);
}

void AccountsWorker::removeUser(const QString &userPath)
{
    for (AccountsUser *userInter : m_userInters.values())
    {
        if (userInter->path() == userPath)
        {
            User *user = m_userInters.key(userInter);

            m_userInters.remove(user);
            m_userModel->removeUser(userPath);

            return;
        }
    }
}

void AccountsWorker::updateUserOnlineStatus(const QList<QDBusObjectPath> paths)
{
    m_onlineUsers.clear();

    for (const QDBusObjectPath &path : paths) {
        Session tmpSession(DisplayManagerService, path.path(), QDBusConnection::systemBus());
        m_onlineUsers << tmpSession.userName();
    }

    for (User *user : m_userModel->userList()) {
        const bool online = m_onlineUsers.contains(user->name());
        user->setOnline(online);
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
        result->setMessage(dgettext("dde-daemon", reply.argumentAt(1).toString().toUtf8().data()));
        return result;
    }

    // validate password
    if (user->password() != user->repeatPassword()) {
        result->setType(CreationResult::PasswordMatchError);
        result->setMessage(tr("Password not match."));
        return result;
    }

    QDBusObjectPath path = m_accountsInter->CreateUser(user->name(), user->name(), 1);
    const QString userPath = path.path();
    if (userPath.isEmpty() || userPath.isNull()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("no method call result on CreateUser");
        return result;
    }

    AccountsUser *userDBus = new AccountsUser("com.deepin.daemon.Accounts", userPath, QDBusConnection::systemBus(), this);
    if (!userDBus->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    //TODO(hualet): better to check all the call results.
    bool sifResult = !userDBus->SetIconFile(user->currentAvatar()).isError();
    bool spResult = !userDBus->SetPassword(user->password()).isError();

    if (!sifResult || !spResult) {
        result->setType(CreationResult::UnknownError);
        if (!sifResult) result->setMessage("set icon file for new created user failed.");
        if (!spResult) result->setMessage("set password for new created user failed");

        return result;
    }

    return result;
}
