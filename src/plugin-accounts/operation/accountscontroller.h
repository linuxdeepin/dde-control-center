// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ACCOUNTSCONTROLLER_H
#define ACCOUNTSCONTROLLER_H

#include "usermodel.h"
#include "accountsworker.h"
#include "creationresult.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QHash>
#include <QtQml/qqml.h>

namespace dccV25 {

class AccountsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentUserName READ currentUserName NOTIFY currentUserNameChanged FINAL)
    Q_PROPERTY(QStringList userIdList READ userIdList NOTIFY userIdListChanged FINAL)
    Q_PROPERTY(QStringList onlineUserList READ onlineUserList NOTIFY onlineUserListChanged FINAL)
public:
    explicit AccountsController(QObject *parent = nullptr);
    virtual ~AccountsController();

    QString currentUserName() const;
    QStringList userIdList() const;
    QStringList onlineUserList() const;

public slots:
    QString currentUserId() const;
    bool curUserIsSysAdmin() const;

    QString avatar(const QString &id) const;
    void setAvatar(const QString &id, const QString &url);
    QStringList avatars(const QString &id, const QString &filter, const QString &section);
    QString userName(const QString &id) const;
    QString fullName(const QString &id) const;
    void setFullname(const QString &id, const QString &name);
    int userType(const QString &id) const;
    void setUserType(const QString &id, int index);
    QString userTypeName(const QString &id) const;
    QStringList userTypes(bool createUser = false) const;
    bool isDeleteAble(const QString &id) const;

    bool isAutoLoginVisable() const;
    bool autoLogin(const QString &id) const;
    void setAutoLogin(const QString &id, const bool enable);
    QString getOtherUserAutoLogin() const;
    bool isQuickLoginVisible() const;
    bool quickLogin(const QString &id) const;
    void setQuickLogin(const QString &id, const bool enable);
    bool isNoPassWordLoginVisable() const;
    bool nopasswdLogin(const QString &id);
    void setNopasswdLogin(const QString &id, const bool enable);
    bool isOnline(const QString &id);
    bool needShowGroups();

    QStringList allGroups() const;
    QStringList groups(const QString &id);
    void updateGroups(const QString &id);
    void updateAllGroups();
    void setGroup(const QString &id, const QString &group, bool on);
    bool groupContains(const QString &id, const QString &name) const;
    bool groupEnabled(const QString &id, const QString &name) const;
    bool groupEditAble(const QString &id, const QString &name) const;
    bool groupExists(const QString &name) const;
    void createGroup(const QString &name);
    void deleteGroup(const QString &name);
    void modifyGroup(const QString &oldName, const QString &newName);

    void addUser(const QVariantMap &userInfo);
    void removeUser(const QString &id, const bool deleteHome);
    void setPassword(const QString &id, const QVariantMap &info);
    void setPasswordHint(const QString &id, const QString &pwdHint);
    int passwordAge(const QString &id) const;
    void setPasswordAge(const QString &id, const int age);

    QSortFilterProxyModel *avatarFilterModel();
    QAbstractListModel *avatarTypesModel();
    QAbstractListModel *accountsModel();
    QAbstractListModel *groupsModel(const QString &id);

    int passwordLevel(const QString &pwd);
    QString checkUsername(const QString &name);
    QString checkFullname(const QString &name);
    QString checkPassword(const QString &name, const QString &pwd);
    QVariantMap checkPasswordResult(int code, const QString &msg, const QString &name, const QString &pwd);
    void showDefender();
    void playSystemSound(int soundType);

    void updateSingleUserGroups(const QString &id);

signals:
    void currentUserNameChanged();
    void userIdListChanged();
    void userRemoved(const QString &userId);
    void onlineUserListChanged();
    void avatarChanged(const QString &userId, const QString &avatar);
    void userTypeChanged(const QString &userId, const int userType);
    void fullnameChanged(const QString &userId, const QString &fullname);
    void autoLoginChanged(const QString &userId, bool enable);
    void quickLoginChanged(const QString &userId, bool enable);
    void nopasswdLoginChanged(const QString &userId, bool enable);
    void passwordAgeChanged(const QString &userId, const int age);
    void passwordModifyFinished(const QString &userId, const int exitCode, const QString &msg);
    void groupsChanged(const QString &userId, const QStringList &groups);
    void groupsUpdate(); // create/delete/modify
    void groupsUpdateFailed(const QString &groupName);
    void requestCreateGroup(const QString &userId);
    void requestClearEmptyGroup(const QString &userId);
    void showSafetyPage(const QString &errorTips);
    void accountCreationFinished(CreationResult::ResultType resultType, const QString &message);
protected:
    bool isSystemAdmin(const User *user) const;
    int adminCount() const;
    void checkPwdLimitLevel(int lvl);
private:
    AccountsWorker *m_worker = nullptr;
    UserModel      *m_model = nullptr;
    QSortFilterProxyModel *m_avatarFilterModel = nullptr;
    QAbstractListModel    *m_avatarTypesModel = nullptr;
    QAbstractListModel    *m_accountsModel = nullptr;
    QHash<QString, QStringList> m_groups;
    QAbstractListModel    *m_groupsModel = nullptr;
    bool m_isCreatingUser = false;
};

}

#endif // ACCOUNTSCONTROLLER_H
