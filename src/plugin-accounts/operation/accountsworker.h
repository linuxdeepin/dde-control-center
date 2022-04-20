/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             Tinalu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             Tinalu Shao <shaotianlu@uniontech.com>
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

#include "interface/namespace.h"
#include "usermodel.h"
#include "creationresult.h"
#include "accountsdbusproxy.h"
#include "userdbusproxy.h"

#include <QObject>

#define SECURITY_QUESTIONS_ERROR_COUNT 1

namespace DCC_NAMESPACE {

class User;

struct BindCheckResult {
    QString ubid = "";
    QString error = "";
};

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = nullptr);

    void active();
    QString getCurrentUserName();
    void updateGroupinfo();
    QDBusPendingReply<bool, QString, int> isUsernameValid(const QString &name);

Q_SIGNALS:
    void accountCreationFinished(CreationResult *result) const;
    void accountFullNameChangeFinished() const;
    void requestMainWindowEnabled(const bool isEnabled) const;
    void localBindUbid(const QString &ubid);
    void localBindError(const QString &error);

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
    void localBindCheck(User *user, const QString &uosid, const QString &uuid);
    void startResetPasswordExec(User *user);
    void asyncSecurityQuestionsCheck(User *user);
    void refreshADDomain();
    void ADDomainHandle(const QString &server, const QString &admin, const QString &password);
    void addUser(const QString &userPath);
    void removeUser(const QString &userPath);
    void setGroups(User *user, const QStringList &usrGroups);
    void setPasswordHint(User *user, const QString &passwordHint);
    void setSecurityQuestions(User *user, const QMap<int, QByteArray> &securityQuestions);

    bool hasOpenSecurity();
    SecurityLever getSecUserLeverbyname(QString userName);
private Q_SLOTS:
    void updateUserOnlineStatus(const QList<QDBusObjectPath> &paths);
    void getAllGroups();
    void getAllGroupsResult(QDBusPendingCallWatcher *watch);
    void getPresetGroups();
    void getPresetGroupsResult(QDBusPendingCallWatcher *watch);
    void checkADUser();

private:
    UserDBusProxy *userInter(const QString &userName) const;
    CreationResult *createAccountInternal(const User *user);
    QString cryptUserPassword(const QString &password);
    BindCheckResult checkLocalBind(const QString &uosid, const QString &uuid);
    QList<int> securityQuestionsCheck();

private:
    AccountsDBusProxy *m_accountsInter;
    UserDBusProxy *m_userQInter;
    QDBusInterface *m_syncHelperInter;
    QMap<User *, UserDBusProxy *> m_userInters;
    QString m_currentUserName;
    QStringList m_onlineUsers;
    UserModel *m_userModel;
};

}   // namespace DCC_NAMESPACE

#endif // ACCOUNTSWORKER_H
