//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "accountsworker.h"
#include "user.h"
#include "syncdbusproxy.h"
#include "accountsdbusproxy.h"
#include "userdbusproxy.h"
#include "securitydbusproxy.h"

#include <ddbussender.h>

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDBusReply>
#include <DSysInfo>

#include <pwd.h>
#include <tuple>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>
#include <PolkitQt1/Authority>
using namespace PolkitQt1;

using namespace dccV25;
DCORE_USE_NAMESPACE

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent)
    , m_accountsInter(new AccountsDBusProxy(this))
    , m_userQInter(new UserDBusProxy(QString("/org/deepin/dde/Accounts1/User%1").arg(getuid()), this))
    , m_syncInter(new SyncDBusProxy(this))
    , m_securityInter(new SecurityDBusProxy(this))
    , m_userModel(userList)
{
    struct passwd *pws;
    pws = getpwuid(getuid());
    m_currentUserName = QString(pws->pw_name);
    m_userModel->setCurrentUserName(m_currentUserName);
    m_userModel->setIsSecurityHighLever(hasOpenSecurity());

    connect(m_accountsInter, &AccountsDBusProxy::UserListChanged, this, &AccountsWorker::onUserListChanged, Qt::QueuedConnection);
    connect(m_accountsInter, &AccountsDBusProxy::GroupListChanged, this, &AccountsWorker::onGroupListChanged, Qt::QueuedConnection);
    connect(m_accountsInter, &AccountsDBusProxy::UserAdded, this, &AccountsWorker::addUser, Qt::QueuedConnection);
    connect(m_accountsInter, &AccountsDBusProxy::UserDeleted, this, &AccountsWorker::removeUser, Qt::QueuedConnection);
    connect(m_accountsInter, &AccountsDBusProxy::SessionsChanged, this, &AccountsWorker::updateUserOnlineStatus);

    QDBusPendingReply<QString> reply  = m_accountsInter->FindUserById(pws->pw_uid);
     QString currentUserPath = reply.value();
    if (!currentUserPath.isEmpty()) {
        onUserListChanged({currentUserPath});
    }
    onUserListChanged(m_accountsInter->userList());
    updateUserOnlineStatus(m_accountsInter->sessions());
    getAllGroups();
    getPresetGroups();

    if(DSysInfo::UosServer != DSysInfo::uosType()) {
        m_userModel->setAutoLoginVisable(true);
        m_userModel->setNoPassWordLoginVisable(true);
    } else {
        m_userModel->setAutoLoginVisable(true);
        m_userModel->setNoPassWordLoginVisable(false);
    }
}

void AccountsWorker::getAllGroups()
{
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetGroups();
    QDBusPendingCallWatcher *groupResult = new QDBusPendingCallWatcher(reply, this);
    connect(groupResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getAllGroupsResult);
}

void AccountsWorker::getGroupInfoByName(const QString &groupName, QString &resInfoJson)
{
    QString info;

    QDBusPendingReply<QString> reply = m_accountsInter->GetGroupInfoByName(groupName);
    QDBusPendingCallWatcher *groupResult = new QDBusPendingCallWatcher(reply, this);
    connect(groupResult, &QDBusPendingCallWatcher::finished, this, [&resInfoJson](QDBusPendingCallWatcher *watch) {
        QDBusPendingReply<QString> reply = *watch;
        if (!watch->isError()) {
            resInfoJson = reply.value();
        } else {
            qDebug() << "getGroupInfoByName error." << watch->error();
        }
        watch->deleteLater();
    });

    groupResult->waitForFinished();
}

void AccountsWorker::getAllGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setAllGroups(reply.value());

        QStringList disabledGroups;
        QJsonDocument jsonDocument;
        QJsonObject jsonObject;
        QString info;

        foreach (auto name, reply.value()) {
            getGroupInfoByName(name, info);
            jsonDocument = QJsonDocument::fromJson(info.toLocal8Bit());
            jsonObject = jsonDocument.object();

            bool res = false;
            int gid = jsonObject.value("Gid").toString().toInt(&res);
            if (res && 1000 > gid) {
                if (!disabledGroups.contains(name))
                    disabledGroups.append(name);
            }
        }
        m_userModel->setDisabledGroups(disabledGroups);
    } else {
        qDebug() << "getAllGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

void AccountsWorker::getPresetGroups()
{
    int userType = DSysInfo::UosServer == DSysInfo::uosType() ? 0 : 1;
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetPresetGroups(userType);
    QDBusPendingCallWatcher *presetGroupsResult = new QDBusPendingCallWatcher(reply, this);
    connect(presetGroupsResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getPresetGroupsResult);
}

void AccountsWorker::getPresetGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setPresetGroups(reply.value());
    } else {
        qDebug() << "getPresetGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

void AccountsWorker::getUOSID(QString &uosid)
{
    const auto &result = m_syncInter->UOSID();
    if (!result.isEmpty()) {
        uosid = result;
    }
}

void AccountsWorker::getUUID(QString &uuid)
{
    QVariant retUUID = m_userQInter->uuid();
    uuid = retUUID.toString();
}

void AccountsWorker::localBindCheck(User *user, const QString &uosid, const QString &uuid)
{
    Q_UNUSED(user)
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT localBindUbid(result.ubid);
        else
            Q_EMIT localBindError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(&AccountsWorker::checkLocalBind, this, uosid, uuid);
    watcher->setFuture(future);
}

void AccountsWorker::startResetPasswordExec(User *user)
{
    qInfo() << "Begin Resetpassword";
    UserDBusProxy *userInter = m_userInters.value(user);
    auto reply = userInter->SetPassword("");
    reply.waitForFinished();
    Q_EMIT user->startResetPasswordReplied(reply.error().message());
}

void AccountsWorker::asyncSecurityQuestionsCheck(User *user)
{
    QFutureWatcher<QList<int>> *watcher = new QFutureWatcher<QList<int>>(this);
    connect(watcher, &QFutureWatcher<QList<int>>::finished, [user, watcher] {
        QList<int> result = watcher->result();
        if (result.size() != SECURITY_QUESTIONS_ERROR_COUNT)
            Q_EMIT user->startSecurityQuestionsCheckReplied(result);

        watcher->deleteLater();
    });
    QFuture<QList<int>> future = QtConcurrent::run(&AccountsWorker::securityQuestionsCheck, this);
    watcher->setFuture(future);
}

QList<int> AccountsWorker::securityQuestionsCheck()
{
    QDBusPendingReply<QList<int>> reply = m_userQInter->GetSecretQuestions();
    if (!reply.error().message().isEmpty()) {
        qWarning() << reply.error().message();
    }
    if (reply.isValid()) {
        return reply.value();
    }
    return {-1};
}

void AccountsWorker::setPasswordHint(User *user, const QString &passwordHint)
{
    UserDBusProxy *userInter = m_userInters.value(user);
    Q_ASSERT(userInter);

    userInter->SetPasswordHint(passwordHint);
}

void AccountsWorker::setSecurityQuestions(User *user, const QMap<int, QByteArray> &securityQuestions)
{
    QDBusPendingReply<void> reply =  m_userQInter->SetSecretQuestions(securityQuestions);
    if (reply.isValid()) {
        Q_EMIT user->setSecurityQuestionsReplied(reply.error().message());
    }
    if (!reply.error().message().isEmpty()) {
        Q_EMIT user->setSecurityQuestionsReplied(reply.error().message() + "error");
    }
}

void AccountsWorker::deleteGroup(const QString &group)
{
    QDBusPendingCall call = m_accountsInter->deleteGroup(group);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, group] (QDBusPendingCallWatcher* call) {
        if (call->isError()) {
            qWarning() << "Delete group " << group << " failed, error:" << call->error().message();
            Q_EMIT updateGroupFailed(group);
            return;
        }
        Q_EMIT updateGroupFinished(AccountsWorker::OperateType::Delete, call->isValid(), group);
    });
}

void AccountsWorker::createGroup(const QString &group, uint32_t gid, bool isSystem)
{
    QDBusPendingCall call = m_accountsInter->createGroup(group, gid, isSystem);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, group, gid] (QDBusPendingCallWatcher* call) {
        if (call->isError()) {
            qWarning() << "Create group, gid: " << gid << ", created group `" << group << "` failed, error:" << call->error().message();
            Q_EMIT createGroupFailed(group);
            return;
        }

        Q_EMIT updateGroupFinished(AccountsWorker::OperateType::Create, call->isValid());
    });
}

void AccountsWorker::modifyGroup(const QString &oldGroup, const QString &newGroup, uint32_t gid)
{
    QDBusPendingCall call = m_accountsInter->modifyGroup(oldGroup, newGroup, gid);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, oldGroup, newGroup] (QDBusPendingCallWatcher* call) {
        if (call->isError()) {
            qWarning() << "Modify group from " << oldGroup << " to " << newGroup << " failed, error:" << call->error().message();
            // 这里返回旧的 groupname， 因为 model 中的数据还没更新，给一个新的是定位不到 model 的 index 来触发 datachanged
            Q_EMIT updateGroupFailed(oldGroup);
            return;
        }

        Q_EMIT updateGroupFinished(AccountsWorker::OperateType::Modify, call->isValid());
    });
}

bool AccountsWorker::hasOpenSecurity()
{
    const auto &value = m_securityInter->Status();
    if (value.isEmpty()) {
        qWarning() << m_securityInter->lastError();
        return false;
    }
    if (value == "open")
        return true;
    return false;
}

SecurityLever AccountsWorker::getSecUserLeverbyname(QString userName)
{
    std::tuple<QString, QString> result = m_securityInter->GetSEUserByName(userName);
    const auto &value = std::get<0>(result);
    if (value.isEmpty()) {
        qWarning() << m_securityInter->lastError();
        return SecurityLever::Standard;
    }

    if (value == QStringLiteral("sysadm_u")) {
        return SecurityLever::Sysadm;
    }
    if (value == QStringLiteral("secadm_u")) {
        return SecurityLever::Secadm;
    }
    if (value == QStringLiteral("audadm_u")) {
        return SecurityLever::Audadm;
    }
    if (value == QStringLiteral("auditadm_u")) {
        return SecurityLever::Auditadm;
    }

    return SecurityLever::Standard;
}

void AccountsWorker::checkPwdLimitLevel(int level)
{
    // 密码校验失败并且安全中心密码安全等级不为低，弹出跳转到安全中心的对话框，低、中、高等级分别对应的值为1、2、3
    QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                             QStringLiteral("/com/deepin/defender/daemonservice"),
                             QStringLiteral("com.deepin.defender.daemonservice"));
    if (!interface.isValid()) {
        return;
    }
    QDBusReply<int> pwdLimitLevel = interface.call("GetPwdLimitLevel");
    if (pwdLimitLevel.error().type() == QDBusError::NoError && pwdLimitLevel > level) {
        QDBusReply<QString> errorTips = interface.call("GetPwdError");
        Q_EMIT showSafetyPage(errorTips);
    }
}

void AccountsWorker::showDefender()
{
    qDebug() << "showDefender call.....";
    QDBusPendingCall call = DDBusSender()
            .service("com.deepin.defender.hmiscreen")
            .interface("com.deepin.defender.hmiscreen")
            .path("/com/deepin/defender/hmiscreen")
            .method(QString("ShowPage"))
            .arg(QString("securitytools"))
            .arg(QString("login-safety"))
            .call();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    watcher->waitForFinished();
    watcher->deleteLater();
}

void AccountsWorker::setGroups(User *user, const QStringList &usrGroups)
{
    UserDBusProxy *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->SetGroups(usrGroups);
}

void AccountsWorker::active()
{
    for (auto it(m_userInters.cbegin()); it != m_userInters.cend(); ++it) {
        it.key()->setName(it.value()->property("UserName").toString());
        it.key()->setAutoLogin(it.value()->automaticLogin());
        it.key()->setNopasswdLogin(it.value()->noPasswdLogin());
        it.key()->setUserType(it.value()->accountType());
        it.key()->setAvatars(it.value()->iconList());
        it.key()->setGroups(it.value()->groups());
        it.key()->setCurrentAvatar(it.value()->iconFile());
        it.key()->setCreatedTime(it.value()->createdTime());
        it.key()->setGid(it.value()->gid());
        it.key()->setFullname(it.value()->fullName());
        it.key()->setIsCurrentUser(it.value()->property("UserName").toString() == m_currentUserName);
        it.key()->setId(it.value()->path());
    }
}

QString AccountsWorker::getCurrentUserName()
{
    return m_currentUserName;
}

QDBusPendingReply<bool, QString, int> AccountsWorker::isUsernameValid(const QString &name)
{
    QDBusPendingReply<bool, QString, int> reply = m_accountsInter->IsUsernameValid(name);
    reply.waitForFinished();
    return reply;
}

void AccountsWorker::randomUserIcon(User *user)
{
    QDBusPendingCall call = m_accountsInter->RandUserIcon();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=] {
        if (!call.isError()) {
            QDBusPendingReply<QString> reply = call.reply();
            user->setCurrentAvatar(reply.value());
        }
        watcher->deleteLater();
    });
}

void AccountsWorker::createAccount(const User *user)
{
    qDebug() << "create account";

    QFutureWatcher<CreationResult *> *watcher = new QFutureWatcher<CreationResult *>(this);
    connect(watcher, &QFutureWatcher<CreationResult *>::finished, [this, watcher, user] {
        CreationResult *result = watcher->result();
        Q_EMIT accountCreationFinished(result);
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });

    QFuture<CreationResult *> future = QtConcurrent::run(&AccountsWorker::createAccountInternal, this, user);
    Q_EMIT requestMainWindowEnabled(false);
    watcher->setFuture(future);
}

void AccountsWorker::updateGroupinfo()
{
    m_userModel->setAllGroups(m_accountsInter->GetGroups());
}

void AccountsWorker::setAvatar(User *user, const QString &iconPath)
{
    qDebug() << "set account avatar";
    UserDBusProxy *ui = m_userInters[user];
    Q_ASSERT(ui);

    ui->SetIconFile(iconPath);
}

void AccountsWorker::setFullname(User *user, const QString &fullname)
{
   qInfo() << "fullname" << fullname;

    UserDBusProxy *ui = m_userInters[user];
    Q_ASSERT(ui);

    QDBusPendingCall call = ui->SetFullName(fullname);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->fullnameChanged(user->fullname());
        } else {
            Q_EMIT accountFullNameChangeFinished();
        }
        watcher->deleteLater();
    });
}

void AccountsWorker::deleteAccount(User *user, const bool deleteHome)
{
    QDBusPendingCall call = m_accountsInter->DeleteUser(user->name(), deleteHome);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, user] (QDBusPendingCallWatcher* call) {
        Q_EMIT requestMainWindowEnabled(true);
        if (call->isError()) {
            qDebug() << Q_FUNC_INFO << call->error().message();
            Q_EMIT m_userModel->isCancelChanged();
        } else {
            if (!m_userInters.contains(user)) {
                call->deleteLater();
                return;
            }
            Q_EMIT m_userModel->deleteUserSuccess();
            removeUser(m_userInters.value(user)->path());
            getAllGroups();
        }
        call->deleteLater();
    });
    Q_EMIT requestMainWindowEnabled(false);
}

void AccountsWorker::setAutoLogin(User *user, const bool autoLogin)
{
    UserDBusProxy *ui = m_userInters[user];
    Q_ASSERT(ui);

    QDBusPendingCall call = ui->SetAutomaticLogin(autoLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->autoLoginChanged(user->autoLogin());
        }
        watcher->deleteLater();
    });
}

//切换账户权限
void AccountsWorker::setAdministrator(User *user, const bool asAdministrator)
{
    UserDBusProxy *ui = m_userInters[user];
    Q_ASSERT(ui);

    // because this operate need root permission, we must wait for finished and refersh result
    Q_EMIT requestMainWindowEnabled(false);

    QStringList lstGroups = ui->groups();
    if(!asAdministrator)
        lstGroups.removeOne("sudo");
    else
        lstGroups.append("sudo");

    QDBusPendingCall call = ui->SetGroups(lstGroups);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->userTypeChanged(user->userType());
        }
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });
}

void AccountsWorker::loadUserList()
{
    onUserListChanged(m_accountsInter->userList());
}

void AccountsWorker::onUserListChanged(const QStringList &userList)
{
    for (const QString &path : userList) {
        if (!m_userModel->contains(path)) {
            addUser(path);
        }
    }
}

void AccountsWorker::onGroupListChanged(const QStringList &groupList)
{
    if (m_userModel)
        m_userModel->setAllGroups(groupList);
}

void AccountsWorker::setPassword(User *user, const QString &oldpwd, const QString &passwd, const QString &repeatPasswd, const bool needResult)
{
    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");
    process.setProcessEnvironment(env);
    process.setProcessChannelMode(QProcess::MergedChannels);

    process.start("/bin/bash", QStringList() << "-c" << QString("passwd"));
    if (user->passwordStatus() == NO_PASSWORD) {
        process.write(QString("%1\n%2\n").arg(passwd).arg(repeatPasswd).toLatin1());
    } else {
        process.write(QString("%1\n%2\n%3").arg(oldpwd).arg(passwd).arg(repeatPasswd).toLatin1());
    }

    process.closeWriteChannel();
    process.waitForFinished();

    if (needResult) {
        // process.exitCode() = 0 表示密码修改成功
        int exitCode = process.exitCode();
        const QString& outputTxt = process.readAll();
        Q_EMIT user->passwordModifyFinished(exitCode, outputTxt);
    }
}

void AccountsWorker::resetPassword(User *user, const QString &password)
{
    auto reply = m_userInters.value(user)->SetPassword(cryptUserPassword(password));
    reply.waitForFinished();

    Q_EMIT user->passwordResetFinished(reply.error().message());
}

void AccountsWorker::deleteUserIcon(User *user, const QString &iconPath)
{
    UserDBusProxy *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->DeleteIconFile(iconPath);
}

void AccountsWorker::addUser(const QString &userPath)
{
    if (userPath.contains("User0", Qt::CaseInsensitive) || m_userModel->contains(userPath))
        return;

    if(!userPath.contains("/org/deepin/dde/Accounts1"))
        return;

    UserDBusProxy *userInter = new UserDBusProxy(userPath, this);
    User *user = new User(this);

    connect(userInter, &UserDBusProxy::UserNameChanged, user, [=](const QString &name) {
        user->setName(name);
        user->setSecurityLever(getSecUserLeverbyname(name));
        user->setOnline(m_onlineUsers.contains(name));
        user->setIsCurrentUser(name == m_currentUserName);
        checkADUser();
    });

    connect(userInter, &UserDBusProxy::AutomaticLoginChanged, user, &User::setAutoLogin);
    connect(userInter, &UserDBusProxy::IconListChanged, user, &User::setAvatars);
    connect(userInter, &UserDBusProxy::IconFileChanged, user, &User::setCurrentAvatar);
    connect(userInter, &UserDBusProxy::FullNameChanged, user, &User::setFullname);
    connect(userInter, &UserDBusProxy::NoPasswdLoginChanged, user, &User::setNopasswdLogin);
    connect(userInter, &UserDBusProxy::PasswordStatusChanged, user, &User::setPasswordStatus);
    connect(userInter, &UserDBusProxy::CreatedTimeChanged, user, &User::setCreatedTime);
    connect(userInter, &UserDBusProxy::GroupsChanged, user, &User::setGroups);
    connect(userInter, &UserDBusProxy::AccountTypeChanged, user, &User::setUserType);
    connect(userInter, &UserDBusProxy::MaxPasswordAgeChanged, user, &User::setPasswordAge);
    connect(userInter, &UserDBusProxy::GidChanged, user, &User::setGid);

    // 这里直接赋值的话, 由于请求是异步的, 所以一开始会被初始化成乱码,
    // 然后数据正常了以后会额外产生一次变化信号
    // 对于计算当前有多少个管理员有干扰.
    userInter->iconList();
    userInter->groups();
    userInter->iconFile();
    userInter->noPasswdLogin();
    userInter->passwordStatus();
    userInter->createdTime();
    userInter->accountType();
    userInter->maxPasswordAge();
    userInter->IsPasswordExpired();
    userInter->gid();

    user->setId(userPath);
    user->setName(userInter->userName());
    user->setFullname(userInter->fullName());
    user->setAutoLogin(userInter->automaticLogin());
    user->setAvatars(userInter->iconList());
    user->setCurrentAvatar(userInter->iconFile());
    user->setNopasswdLogin(userInter->noPasswdLogin());
    user->setPasswordStatus(userInter->passwordStatus());
    user->setCreatedTime(userInter->createdTime());
    user->setGroups(userInter->groups());
    user->setUserType(userInter->accountType());
    user->setPasswordAge(userInter->maxPasswordAge());
    user->setGid(userInter->gid());

    m_userInters[user] = userInter;
    m_userModel->addUser(userPath, user);
}

void AccountsWorker::removeUser(const QString &userPath)
{
    for (UserDBusProxy *userInter : m_userInters.values()) {
        if (userInter->path() == userPath) {
            User *user = m_userInters.key(userInter);
            user->deleteLater();

            m_userInters.remove(user);
            m_userModel->removeUser(userPath);

            return;
        }
    }
}

void AccountsWorker::setNopasswdLogin(User *user, const bool nopasswdLogin)
{
    UserDBusProxy *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    QDBusPendingCall call = userInter->EnableNoPasswdLogin(nopasswdLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->nopasswdLoginChanged(user->nopasswdLogin());
        }
        QProcess restartLock;
        QStringList restartLockCommand = QStringList { "--user", "restart", "dde-lock.service" };
        restartLock.start("systemctl", restartLockCommand);
        restartLock.waitForFinished(-1);
        watcher->deleteLater();
    });
}

void AccountsWorker::setMaxPasswordAge(User *user, const int maxAge)
{
    UserDBusProxy *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    QDBusPendingCall call = userInter->SetMaxPasswordAge(maxAge);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->passwordAgeChanged(user->passwordAge());
        }
        watcher->deleteLater();
    });
}

void AccountsWorker::refreshADDomain()
{
    QProcess *process = new QProcess(this);
    process->start("/opt/pbis/bin/enum-users", QStringList());

    connect(process, &QProcess::readyReadStandardOutput, this, [=] {
        QRegularExpression re("Name:\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(process->readAll());
        m_userModel->setIsJoinADDomain(match.hasMatch());
    });

    connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), process, &QProcess::deleteLater);
}

void AccountsWorker::ADDomainHandle(const QString &server, const QString &admin, const QString &password)
{
    const bool isJoin = m_userModel->isJoinADDomain();
    int exitCode = 0;
    if (isJoin) {
        exitCode = QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/domainjoin-cli"
                                                             << "leave"
                                                             << "--disable"
                                                             << "ssh");
    } else {
        // for safety, restart lwsmd service before join AD Domain
        QProcess::execute("pkexec", QStringList() << "/bin/systemctl"
                                                  << "restart"
                                                  << "lwsmd");
        exitCode = QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/domainjoin-cli"
                                                             << "join"
                                                             << "--disable"
                                                             << "ssh" << server << admin << password);
    }

    QString message;

    if (!exitCode) {
        message = isJoin ? tr("Your host was removed from the domain server successfully")
                         : tr("Your host joins the domain server successfully");

        // Additional operation, need to initialize the user's settings
        if (!isJoin) {
            QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/config"
                                                      << "UserDomainPrefix"
                                                      << "ADS");
            QProcess::execute("pkexec", QStringList() << "/opt/pbis/bin/config"
                                                      << "LoginShellTemplate"
                                                      << "/bin/bash");
        }
        // save config
        QFile file("/etc/deepin/dde-session-ui.conf");
        QFile tmpFile("/tmp/.dde-session-ui.conf");

        if (file.exists() && file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            qDebug() << file.copy("/tmp/.dde-session-ui.conf");
        }
        if (tmpFile.open(QIODevice::Text | QIODevice::ReadWrite)) {
            QSettings setting("/tmp/.dde-session-ui.conf", QSettings::IniFormat);
            setting.setValue("loginPromptInput", !isJoin);
            setting.sync();
            QProcess::execute("pkexec", QStringList() << "cp"
                                                      << "/tmp/.dde-session-ui.conf"
                                                      << "/etc/deepin/dde-session-ui.conf");
            tmpFile.remove();
        }
    } else {
        message = isJoin ? tr("Your host failed to leave the domain server")
                         : tr("Your host failed to join the domain server");
    }

    DDBusSender()
        .service("org.freedesktop.Notifications")
        .path("/org/freedesktop/Notifications")
        .interface("org.freedesktop.Notifications")
        .method("Notify")
        .arg(QString())
        .arg((uint)QDateTime::currentMSecsSinceEpoch())
        .arg(exitCode ? QStringLiteral("dialog-warning") : QStringLiteral("dialog-ok"))
        .arg(tr("AD domain settings"))
        .arg(message)
        .arg(QStringList())
        .arg(QVariantMap())
        .arg((int)0)
        .call();

    refreshADDomain();
}

void AccountsWorker::updateUserOnlineStatus(const QList<QDBusObjectPath> &paths)
{
    m_onlineUsers.clear();
    m_userModel->SetOnlineUsers(QStringList());

    for (const QDBusObjectPath &path : paths) {
        QDBusInterface sessionInter("org.freedesktop.DisplayManager",
                                    path.path(),
                                    "org.freedesktop.DisplayManager.Session",
                                    QDBusConnection::systemBus());

        m_onlineUsers << qvariant_cast<QString>(sessionInter.property("UserName"));
    }

    for (User *user : m_userModel->userList()) {
        user->setOnline(m_onlineUsers.contains(user->name()));
    }

    m_userModel->SetOnlineUsers(m_onlineUsers);

    checkADUser();
}

void AccountsWorker::checkADUser()
{
    // AD User is not in native user list, but session list have it.
    bool isADUser = false;

    QStringList userList;

    for (User *user : m_userModel->userList()) {
        userList << user->name();
    }

    for (const QString &u : m_onlineUsers) {
        if (!userList.contains(u)) {
            isADUser = true;
            break;
        }
    }

    m_userModel->setADUserLogind(isADUser);
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
        result->setMessage(tr("Password not match"));
        return result;
    }

    Authority::Result authenticationResult;
    authenticationResult = Authority::instance()->checkAuthorizationSync("org.deepin.dde.accounts.user-administration", UnixProcessSubject(getpid()),
                                                           Authority::AllowUserInteraction);

    if (Authority::Result::Yes != authenticationResult) {
        result->setType(CreationResult::Canceled);
        return result;
    }

    // default FullName is empty string
    QDBusObjectPath path;
    QDBusPendingReply<QDBusObjectPath> createReply = m_accountsInter->CreateUser(user->name(), user->fullname(), user->userType());
    createReply.waitForFinished();
    if (createReply.isError()) {
        /* 这里由后端保证出错时一定有错误信息返回，如果没有错误信息，就默认用户在认证时点了取消 */
        result->setType(createReply.error().message().isEmpty() ? CreationResult::Canceled : CreationResult::UnknownError);
        result->setMessage(createReply.error().message());
        return result;
    } else {
        path = createReply.argumentAt<0>();
    }
    const QString userPath = path.path();

    UserDBusProxy *userDBus = new UserDBusProxy(userPath, this);
    if (!userDBus->interface()->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    //TODO(hualet): better to check all the call results.
    bool sifResult = !userDBus->SetIconFile(user->currentAvatar()).isError();
    bool spResult = !userDBus->SetPassword(cryptUserPassword(user->password())).isError();
    bool groupResult = true;
    bool passwordHintResult = true;
    if (DSysInfo::UosServer == DSysInfo::uosType() && !user->groups().isEmpty()) {
        groupResult = !userDBus->SetGroups(user->groups()).isError();
    }
    passwordHintResult = !userDBus->SetPasswordHint(user->passwordHint()).isError();

    if (!sifResult || !spResult || !groupResult || !passwordHintResult) {
        result->setType(CreationResult::UnknownError);
        if (!sifResult)
            result->setMessage("set icon file for new created user failed.");
        if (!spResult)
            result->setMessage("set password for new created user failed");
        if (!groupResult)
            result->setMessage("set group for new created user failed");
        return result;
    }

    return result;
}

QString AccountsWorker::cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */

    const QString seedchars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedchars.size() - 1); //seedchars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedchars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}

BindCheckResult AccountsWorker::checkLocalBind(const QString &uosid, const QString &uuid)
{
    BindCheckResult result;
    const auto &ret = m_syncInter->LocalBindCheck(uosid, uuid);
    if (!ret.isEmpty())
        result.ubid = ret;
    else
        result.error = m_syncInter->lastError();
    return result;
}
