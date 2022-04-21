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

#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>
#include <org_freedesktop_displaymanager.h>
#include <org_freedesktop_displaymanager_session.h>
#include <com_deepin_daemon_authenticate_fingerprint.h>

#ifdef DCC_ENABLE_ADDOMAIN
#include <org_freedesktop_notifications.h>
#endif

#include "usermodel.h"
#include "creationresult.h"

using Accounts = com::deepin::daemon::Accounts;
using AccountsUser = com::deepin::daemon::accounts::User;
using CreationResult = dcc::accounts::CreationResult;
using Fingerprint = com::deepin::daemon::authenticate::Fingerprint;

using DisplayManager = org::freedesktop::DisplayManager;
using Session = org::freedesktop::displaymanager::Session;

#ifdef DCC_ENABLE_ADDOMAIN
using Notifications = org::freedesktop::Notifications;
#endif
#define SECURITY_QUESTIONS_ERROR_COUNT 1

namespace dcc {
namespace accounts {

class User;

struct BindCheckResult {
    QString ubid = "";
    QString error = "";
};

typedef QMap<int, QByteArray> SecurityQuestions;

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = 0);

    void active();
    QString getCurrentUserName();
    void updateGroupinfo();
    QDBusPendingReply<bool, QString, int> isUsernameValid(const QString &name);

Q_SIGNALS:
    void requestFrameAutoHide(const bool autoHide) const;
    void accountCreationFinished(CreationResult *result) const;
    void accountFullNameChangeFinished() const;
    void requestMainWindowEnabled(const bool isEnabled) const;
    void localBindUbid(const QString &ubid);
    void localBindError(const QString &error);
    void showSafeyPage(const QString &errorTips);

public Q_SLOTS:
    void randomUserIcon(User *user);
    void createAccount(const User *user);

    void setAvatar(User *user, const QString &iconPath);
    void setFullname(User *user, const QString &fullname);
    void deleteAccount(User *user, const bool deleteHome);
    void setAutoLogin(User *user, const bool autoLogin);
    void setAdministrator(User *user, const bool asAdministrator);
    void onUserListChanged(const QStringList &userList);
    void setPassword(User *user, const QString &oldpwd, const QString &passwd, const QString &repeatPasswd,  const bool needResule = true);
    void resetPassword(User *user, const QString &password);
    void deleteUserIcon(User *user, const QString &iconPath);
    void setNopasswdLogin(User *user, const bool nopasswdLogin);
    void setMaxPasswordAge(User *user, const int maxAge);
    void loadUserList();
    void getUOSID(QString &uosid);
    void getUUID(QString &uuid);
    void localBindCheck(dcc::accounts::User *user, const QString &uosid, const QString &uuid);
    void startResetPasswordExec(dcc::accounts::User *user);
    void asyncSecurityQuestionsCheck(dcc::accounts::User *user);
#ifdef DCC_ENABLE_ADDOMAIN
    void refreshADDomain();
    void ADDomainHandle(const QString &server, const QString &admin, const QString &password);
#endif
    void addUser(const QString &userPath);
    void removeUser(const QString &userPath);
    void setGroups(User *user, const QStringList &usrGroups);
    void setPasswordHint(User *user, const QString &passwordHint);
    void setSecurityQuestions(User *user, const QMap<int, QByteArray> &securityQuestions);

    bool hasOpenSecurity();
    SecurityLever getSecUserLeverbyname(QString userName);
    void checkPwdLimitLevel();
private Q_SLOTS:
    void updateUserOnlineStatus(const QList<QDBusObjectPath> &paths);
    void getAllGroups();
    void getAllGroupsResult(QDBusPendingCallWatcher *watch);
    void getPresetGroups();
    void getPresetGroupsResult(QDBusPendingCallWatcher *watch);
#ifdef DCC_ENABLE_ADDOMAIN
    void checkADUser();
#endif

private:
    AccountsUser *userInter(const QString &userName) const;
    CreationResult *createAccountInternal(const User *user);
    QString cryptUserPassword(const QString &password);
    BindCheckResult checkLocalBind(const QString &uosid, const QString &uuid);
    QList<int> securityQuestionsCheck();

private:
    Accounts *m_accountsInter;
    QDBusInterface *m_syncHelperInter;
    QDBusInterface *m_userQInter;
    Fingerprint *m_fingerPrint;
#ifdef DCC_ENABLE_ADDOMAIN
    Notifications *m_notifyInter;
#endif
    QMap<User *, AccountsUser *> m_userInters;
    QString m_currentUserName;
    DisplayManager *m_dmInter;
    QStringList m_onlineUsers;
    UserModel *m_userModel;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWORKER_H
