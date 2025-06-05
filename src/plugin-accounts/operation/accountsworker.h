//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include "usermodel.h"
#include "creationresult.h"

#include <QObject>
#include <QDBusPendingReply>

#define SECURITY_QUESTIONS_ERROR_COUNT 1

class AccountsDBusProxy;
class UserDBusProxy;
class SyncDBusProxy;
class SecurityDBusProxy;

namespace dccV25 {

class User;

struct BindCheckResult {
    QString ubid = "";
    QString error = "";
};

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    enum class OperateType {
        Delete,
        Create,
        Modify
    };

    explicit AccountsWorker(UserModel * userList, QObject *parent = nullptr);

    Q_INVOKABLE void active();
    QString getCurrentUserName();
    void updateGroupinfo();
    QDBusPendingReply<bool, QString, int> isUsernameValid(const QString &name);

Q_SIGNALS:
    void accountCreationFinished(CreationResult *result) const;
    void accountFullNameChangeFinished() const;
    void requestMainWindowEnabled(const bool isEnabled) const;
    void localBindUbid(const QString &ubid);
    void localBindError(const QString &error);
    void showSafetyPage(const QString &errorTips);
    void updateGroupFinished(OperateType operation, bool successfully, const QString& groupName = QString());
    void updateGroupFailed(const QString& groupName = QString());
    void createGroupFailed(const QString& groupName = QString());
public Q_SLOTS:
    void randomUserIcon(User *user);
    void createAccount(const User *user);

    void setAvatar(User *user, const QString &iconPath);
    void setFullname(User *user, const QString &fullname);
    void deleteAccount(User *user, const bool deleteHome);
    void setAutoLogin(User *user, const bool autoLogin);
    void setQuickLogin(User *user, const bool quickLogin);
    void setAdministrator(User *user, const bool asAdministrator);
    void onUserListChanged(const QStringList &userList);
    void onGroupListChanged(const QStringList &groupList);
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
    void deleteGroup(const QString &group);
    void createGroup(const QString &group, uint32_t gid, bool isSystem);
    void modifyGroup(const QString &oldGroup, const QString &newGroup, uint32_t gid);
    void getGroupInfoByName(const QString &groupName, QString &resInfoJson);

    bool hasOpenSecurity();
    SecurityLever getSecUserLeverbyname(QString userName);
    void checkPwdLimitLevel(int level);
    void showDefender();
    void playSystemSound(int soundType);

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
    SyncDBusProxy *m_syncInter;
    SecurityDBusProxy *m_securityInter;
    QMap<User *, UserDBusProxy *> m_userInters;
    QString m_currentUserName;
    QStringList m_onlineUsers;
    UserModel *m_userModel;
};

}   // namespace dccV25

#endif // ACCOUNTSWORKER_H
