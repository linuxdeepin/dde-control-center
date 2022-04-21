/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "accountsworker.h"
#include "window/utils.h"
#include "widgets/utils.h"

#include <QFileDialog>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QGSettings>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>
#include <polkit-qt5-1/PolkitQt1/Authority>

using namespace PolkitQt1;
using namespace dcc::accounts;
using namespace DCC_NAMESPACE;

const QString AccountsService("com.deepin.daemon.Accounts");
const QString FingerPrintService("com.deepin.daemon.Authenticate");
const QString DisplayManagerService("org.freedesktop.DisplayManager");

const QString AutoLoginVisable = "auto-login-visable";
const QString NoPasswordVisable = "nopasswd-login-visable";
const QString Sysadm_u = "sysadm_u";
const QString Secadm_u = "secadm_u";
const QString Audadm_u = "audadm_u";
const QString Auditadm_u = "auditadm_u";

AccountsWorker::AccountsWorker(UserModel *userList, QObject *parent)
    : QObject(parent)
    , m_accountsInter(new Accounts(AccountsService, "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this))
    , m_syncHelperInter(new QDBusInterface("com.deepin.sync.Helper", "/com/deepin/sync/Helper", "com.deepin.sync.Helper", QDBusConnection::systemBus(), this))
    , m_userQInter(new QDBusInterface("com.deepin.daemon.Accounts", QString("/com/deepin/daemon/Accounts/User%1").arg(getuid()), "com.deepin.daemon.Accounts.User", QDBusConnection::systemBus(), this))
    , m_fingerPrint(new Fingerprint(FingerPrintService, "/com/deepin/daemon/Authenticate/Fingerprint", QDBusConnection::systemBus(), this))
#ifdef DCC_ENABLE_ADDOMAIN
    , m_notifyInter(new Notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus(), this))
#endif
    , m_dmInter(new DisplayManager(DisplayManagerService, "/org/freedesktop/DisplayManager", QDBusConnection::systemBus(), this))
    , m_userModel(userList)
{
    qRegisterMetaType<SecurityQuestions>("SecurityQuestions");
    qDBusRegisterMetaType<SecurityQuestions>();
    struct passwd *pws;
    pws = getpwuid(getuid());
    m_currentUserName = QString(pws->pw_name);
    m_userModel->setCurrentUserName(m_currentUserName);
    m_userModel->setIsSecurityHighLever(hasOpenSecurity());

    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsWorker::onUserListChanged, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserAdded, this, &AccountsWorker::addUser, Qt::QueuedConnection);
    connect(m_accountsInter, &Accounts::UserDeleted, this, &AccountsWorker::removeUser, Qt::QueuedConnection);

    connect(m_dmInter, &DisplayManager::SessionsChanged, this, &AccountsWorker::updateUserOnlineStatus);

    m_accountsInter->setSync(false);
    m_dmInter->setSync(false);
#ifdef DCC_ENABLE_ADDOMAIN
    m_notifyInter->setSync(false);
#endif
    QDBusInterface interface(AccountsService, "/com/deepin/daemon/Accounts", AccountsService, QDBusConnection::systemBus());
    QList<QVariant> currentUserPath = interface.call("FindUserById", QString::number(pws->pw_uid)).arguments();
    if (!currentUserPath.isEmpty()) {
        onUserListChanged({currentUserPath.first().toString()});
    }
    onUserListChanged(interface.property("UserList").toStringList());
    updateUserOnlineStatus(m_dmInter->sessions());
    getAllGroups();
    getPresetGroups();

    // 非服务器版本关联gsetting自动登陆/无密码登陆配置
    if (!IsServerSystem) {
        QGSettings *gsetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);

        m_userModel->setAutoLoginVisable(gsetting->get(AutoLoginVisable).toBool());
        m_userModel->setNoPassWordLoginVisable(gsetting->get(NoPasswordVisable).toBool());
        connect(gsetting, &QGSettings::changed, m_userModel, [ = ](const QString &key) {
            if (key == "autoLoginVisable") {
                m_userModel->setAutoLoginVisable(gsetting->get(AutoLoginVisable).toBool());
            } else if (key == "nopasswdLoginVisable") {
                m_userModel->setNoPassWordLoginVisable(gsetting->get(NoPasswordVisable).toBool());
            }
        });
    } else {
        m_userModel->setAutoLoginVisable(true);
        m_userModel->setNoPassWordLoginVisable(false);
    }

    bool bShowCreateUser = valueByQSettings<bool>(DCC_CONFIG_FILES, "", "showCreateUser", true);
    m_userModel->setCreateUserValid(bShowCreateUser);
}

void AccountsWorker::getAllGroups()
{
    QDBusPendingReply<QStringList> reply = m_accountsInter->GetGroups();
    QDBusPendingCallWatcher *groupResult = new QDBusPendingCallWatcher(reply, this);
    connect(groupResult, &QDBusPendingCallWatcher::finished, this, &AccountsWorker::getAllGroupsResult);
}

void AccountsWorker::getAllGroupsResult(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    if (!watch->isError()) {
        m_userModel->setAllGroups(reply.value());
    } else {
        qDebug() << "getAllGroupsResult error." << watch->error();
    }
    watch->deleteLater();
}

void AccountsWorker::getPresetGroups()
{
    int userType = DCC_NAMESPACE::IsServerSystem ? 0 : 1;
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
    if (!m_syncHelperInter->isValid()) {
        qWarning() << "syncHelper interface invalid: (getUOSID)" << m_syncHelperInter->lastError().message();
        return;
    }
    QDBusReply<QString> retUOSID = m_syncHelperInter->call("UOSID");
    if (retUOSID.error().message().isEmpty()) {
        uosid = retUOSID.value();
    } else {
        qWarning() << "UOSID failed:" << retUOSID.error().message();
        return;
    }
}

void AccountsWorker::getUUID(QString &uuid)
{
    QDBusInterface accountsInter("com.deepin.daemon.Accounts",
                                 QString("/com/deepin/daemon/Accounts/User%1").arg(getuid()),
                                 "com.deepin.daemon.Accounts.User",
                                 QDBusConnection::systemBus());
    if (!accountsInter.isValid()) {
        qWarning() << "accountsInter invalid: (getUUID)" << accountsInter.lastError().message();
        return;
    }
    QVariant retUUID = accountsInter.property("UUID");
    uuid = retUUID.toString();
}

void AccountsWorker::localBindCheck(dcc::accounts::User *user, const QString &uosid, const QString &uuid)
{
    QFutureWatcher<BindCheckResult> *watcher = new QFutureWatcher<BindCheckResult>(this);
    connect(watcher, &QFutureWatcher<BindCheckResult>::finished, [this, watcher] {
        BindCheckResult result = watcher->result();
        if (result.error.isEmpty())
            Q_EMIT localBindUbid(result.ubid);
        else
            Q_EMIT localBindError(result.error);
        watcher->deleteLater();
    });
    QFuture<BindCheckResult> future = QtConcurrent::run(this, &AccountsWorker::checkLocalBind, uosid, uuid);
    watcher->setFuture(future);
}

void AccountsWorker::startResetPasswordExec(User *user)
{
    qDebug() << "Begin Resetpassword";
    AccountsUser *userInter = m_userInters.value(user);
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
    QFuture<QList<int>> future = QtConcurrent::run(this, &AccountsWorker::securityQuestionsCheck);
    watcher->setFuture(future);
}

QList<int> AccountsWorker::securityQuestionsCheck()
{
    QDBusReply<QList<int>> reply = m_userQInter->call("GetSecretQuestions");
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
    AccountsUser *userInter = m_userInters.value(user);
    Q_ASSERT(userInter);

    userInter->SetPasswordHint(passwordHint);
}

void AccountsWorker::setSecurityQuestions(User *user, const QMap<int, QByteArray> &securityQuestions)
{
    QDBusReply<void> reply =  m_userQInter->call("SetSecretQuestions", QVariant::fromValue(securityQuestions));
    if (reply.isValid()) {
        Q_EMIT user->setSecurityQuestionsReplied(reply.error().message());
    }
    if (!reply.error().message().isEmpty()) {
        Q_EMIT user->setSecurityQuestionsReplied(reply.error().message() + "error");
    }
}

bool AccountsWorker::hasOpenSecurity()
{
    QDBusInterface securityEnhance("com.deepin.daemon.SecurityEnhance",
                            "/com/deepin/daemon/SecurityEnhance",
                            "com.deepin.daemon.SecurityEnhance",
                            QDBusConnection::systemBus());
    QDBusMessage reply = securityEnhance.call("Status");
    qDebug() << reply.errorMessage();
    QList<QVariant> outArgs = reply.arguments();
    if (outArgs.count() > 0) {
        QString value  = outArgs.at(0).toString();
        if (value == "open") {
            return true;
        }
    }

    return false;
}

SecurityLever AccountsWorker::getSecUserLeverbyname(QString userName)
{
    QDBusInterface securityEnhance("com.deepin.daemon.SecurityEnhance",
                            "/com/deepin/daemon/SecurityEnhance",
                            "com.deepin.daemon.SecurityEnhance",
                            QDBusConnection::systemBus());

    QList<QVariant> currentUserSeName = securityEnhance.call("GetSEUserByName", userName).arguments();

    if (currentUserSeName.count() > 0) {
        QString value  = currentUserSeName.first().toString();
        if (value == Sysadm_u) {
            return SecurityLever::Sysadm;
        }
        if (value == Secadm_u) {
            return SecurityLever::Secadm;
        }
        if (value == Audadm_u) {
            return SecurityLever::Audadm;
        }
        if (value == Auditadm_u) {
            return SecurityLever::Auditadm;
        }
    }

    return SecurityLever::Standard;
}

void AccountsWorker::setGroups(User *user, const QStringList &usrGroups)
{
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->SetGroups(usrGroups);
}

void AccountsWorker::active()
{
    for (auto it(m_userInters.cbegin()); it != m_userInters.cend(); ++it) {
        it.key()->setName(it.value()->property("UserName").toString());
        it.key()->setAutoLogin(it.value()->automaticLogin());
        it.key()->setUserType(it.value()->accountType());
        it.key()->setAvatars(it.value()->iconList());
        it.key()->setGroups(it.value()->groups());
        it.key()->setCurrentAvatar(it.value()->iconFile());
        it.key()->setCreatedTime(it.value()->createdTime());
        it.key()->setGid(it.value()->gid());
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
            QDBusReply<QString> reply = call.reply();
            user->setCurrentAvatar(reply.value());
        }
        watcher->deleteLater();
    });
}

void AccountsWorker::createAccount(const User *user)
{
    qDebug() << "create account";
    Q_EMIT requestFrameAutoHide(false);

    QFutureWatcher<CreationResult *> *watcher = new QFutureWatcher<CreationResult *>(this);
    connect(watcher, &QFutureWatcher<CreationResult *>::finished, [this, watcher] {
        CreationResult *result = watcher->result();
        m_userModel->setAllGroups(m_accountsInter->GetGroups());
        Q_EMIT accountCreationFinished(result);
        Q_EMIT requestFrameAutoHide(true);
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });

    QFuture<CreationResult *> future = QtConcurrent::run(this, &AccountsWorker::createAccountInternal, user);
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
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    ui->SetIconFile(iconPath);
}

void AccountsWorker::setFullname(User *user, const QString &fullname)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = ui->SetFullName(fullname);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (!call.isError()) {
            Q_EMIT accountFullNameChangeFinished();
        }

        Q_EMIT requestFrameAutoHide(true);
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

            QDBusPendingReply<> listFingersReply = m_fingerPrint->ListFingers(user->name());
            listFingersReply.waitForFinished();
            if (listFingersReply.isError()) {
                qDebug() << Q_FUNC_INFO << listFingersReply.error().message();
            } else {
                if (m_fingerPrint->ListFingers(user->name()).value().count()) {
                    QDBusPendingReply<> delAllFingereply = m_fingerPrint->DeleteAllFingers(user->name());
                    delAllFingereply.waitForFinished();
                    if (delAllFingereply.isError()) {
                        qDebug() << Q_FUNC_INFO << delAllFingereply.error().message();
                    }
                }
            }
        }
        call->deleteLater();
    });
    Q_EMIT requestMainWindowEnabled(false);
}

void AccountsWorker::setAutoLogin(User *user, const bool autoLogin)
{
    AccountsUser *ui = m_userInters[user];
    Q_ASSERT(ui);

    // because this operate need root permission, we must wait for finished and refersh result
    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = ui->SetAutomaticLogin(autoLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->autoLoginChanged(user->autoLogin());
        }

        Q_EMIT requestFrameAutoHide(true);
        watcher->deleteLater();
    });
}

//切换账户权限
void AccountsWorker::setAdministrator(User *user, const bool asAdministrator)
{
    AccountsUser *ui = m_userInters[user];
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
    int count = 0;
    static bool first = true;
    for (const QString &path : userList) {
        if (!m_userModel->contains(path)) {
            count++;
            addUser(path);
            if (count > 50 && first) {
                first = false;
                break; // 第一次打开用户列表只加载51个用户信息
            } else if (count > 2 && !first) {
                break; // 滚动条每滑动一次，加载3个用户
            }
        }
    }
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
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    userInter->DeleteIconFile(iconPath);
}

void AccountsWorker::addUser(const QString &userPath)
{
    if (userPath.contains("User0", Qt::CaseInsensitive) || m_userModel->contains(userPath))
        return;
    AccountsUser *userInter = new AccountsUser(AccountsService, userPath, QDBusConnection::systemBus(), this);
    userInter->setSync(false);

    User *user = new User(this);

    connect(userInter, &AccountsUser::UserNameChanged, user, [=](const QString &name) {
        user->setName(name);
        user->setSecurityLever(getSecUserLeverbyname(name));
        user->setOnline(m_onlineUsers.contains(name));
        user->setIsCurrentUser(name == m_currentUserName);
#ifdef DCC_ENABLE_ADDOMAIN
        checkADUser();
#endif
    });

    connect(userInter, &AccountsUser::AutomaticLoginChanged, user, &User::setAutoLogin);
    connect(userInter, &AccountsUser::IconListChanged, user, &User::setAvatars);
    connect(userInter, &AccountsUser::IconFileChanged, user, &User::setCurrentAvatar);
    connect(userInter, &AccountsUser::FullNameChanged, user, &User::setFullname);
    connect(userInter, &AccountsUser::NoPasswdLoginChanged, user, &User::setNopasswdLogin);
    connect(userInter, &AccountsUser::PasswordStatusChanged, user, &User::setPasswordStatus);
    connect(userInter, &AccountsUser::CreatedTimeChanged, user, &User::setCreatedTime);
    connect(userInter, &AccountsUser::GroupsChanged, user, &User::setGroups);
    connect(userInter, &AccountsUser::AccountTypeChanged, user, &User::setUserType);
    connect(userInter, &AccountsUser::MaxPasswordAgeChanged, user, &User::setPasswordAge);
    connect(userInter, &AccountsUser::GidChanged, user, &User::setGid);

    // 这里直接赋值的话, 由于请求是异步的, 所以一开始会被初始化成乱码,
    // 然后数据正常了以后会额外产生一次变化信号
    // 对于计算当前有多少个管理员有干扰.
    userInter->userName();
    userInter->fullName();
    userInter->automaticLogin();
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

    m_userInters[user] = userInter;
    m_userModel->addUser(userPath, user);
}

void AccountsWorker::removeUser(const QString &userPath)
{
    for (AccountsUser *userInter : m_userInters.values()) {
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
    AccountsUser *userInter = m_userInters[user];
    Q_ASSERT(userInter);

    Q_EMIT requestFrameAutoHide(false);

    QDBusPendingCall call = userInter->EnableNoPasswdLogin(nopasswdLogin);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT user->nopasswdLoginChanged(user->nopasswdLogin());
        }

        Q_EMIT requestFrameAutoHide(true);
        watcher->deleteLater();
    });
}

void AccountsWorker::setMaxPasswordAge(User *user, const int maxAge)
{
    AccountsUser *userInter = m_userInters[user];
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

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWorker::refreshADDomain()
{
    QProcess *process = new QProcess(this);
    process->start("/opt/pbis/bin/enum-users");

    connect(process, &QProcess::readyReadStandardOutput, this, [=] {
        QRegularExpression re("Name:\\s+(\\w+)");
        QRegularExpressionMatch match = re.match(process->readAll());
        m_userModel->setIsJoinADDomain(match.hasMatch());
    });

    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), process, &QProcess::deleteLater);
}
#endif

#ifdef DCC_ENABLE_ADDOMAIN
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

    m_notifyInter->Notify("", QDateTime::currentMSecsSinceEpoch(), exitCode ? "dialog-warning" : "dialog-ok", tr("AD domain settings"), message, QStringList(), QVariantMap(), 0);
    refreshADDomain();
}
#endif

void AccountsWorker::updateUserOnlineStatus(const QList<QDBusObjectPath> &paths)
{
    m_onlineUsers.clear();

    for (const QDBusObjectPath &path : paths) {
        Session tmpSession(DisplayManagerService, path.path(), QDBusConnection::systemBus());
        m_onlineUsers << tmpSession.userName();
    }

    for (User *user : m_userModel->userList()) {
        user->setOnline(m_onlineUsers.contains(user->name()));
    }

#ifdef DCC_ENABLE_ADDOMAIN
    checkADUser();
#endif
}

#ifdef DCC_ENABLE_ADDOMAIN
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
#endif

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
    authenticationResult = Authority::instance()->checkAuthorizationSync("com.deepin.daemon.accounts.user-administration", UnixProcessSubject(getpid()),
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

    AccountsUser *userDBus = new AccountsUser("com.deepin.daemon.Accounts", userPath, QDBusConnection::systemBus(), this);
    if (!userDBus->isValid()) {
        result->setType(CreationResult::UnknownError);
        result->setMessage("user dbus is still not valid.");

        return result;
    }

    //TODO(hualet): better to check all the call results.
    bool sifResult = !userDBus->SetIconFile(user->currentAvatar()).isError();
    bool spResult = !userDBus->SetPassword(cryptUserPassword(user->password())).isError();
    bool groupResult = true;
    bool passwordHintResult = true;
    if (IsServerSystem && !user->groups().isEmpty()) {
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
    QDBusReply<QString> retLocalBindCheck= m_syncHelperInter->call(QDBus::BlockWithGui, "LocalBindCheck", uosid, uuid);
    if (!m_syncHelperInter->isValid()) {
        qWarning() << "syncHelper interface invalid: (localBindCheck)" << m_syncHelperInter->lastError().message();
        return result;
    }
    if (retLocalBindCheck.error().message().isEmpty()) {
        result.ubid = retLocalBindCheck.value();
    } else {
        qWarning() << "localBindCheck failed:" << retLocalBindCheck.error().message();
        result.error = retLocalBindCheck.error().message();
    }
    return result;
}

void AccountsWorker::checkPwdLimitLevel()
{
    // 密码校验失败并且安全中心密码安全等级不为低，弹出跳转到安全中心的对话框，低、中、高等级分别对应的值为1、2、3
    QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                             QStringLiteral("/com/deepin/defender/daemonservice"),
                             QStringLiteral("com.deepin.defender.daemonservice"));
    if (!interface.isValid()) {
        return;
    }
    QDBusReply<int> level = interface.call("GetPwdLimitLevel");
    if (level.error().type() == QDBusError::NoError && level != 1) {
        QDBusReply<QString> errorTips = interface.call("GetPwdError");
        Q_EMIT showSafeyPage(errorTips);
    }
}
