//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef USERMODEL_H
#define USERMODEL_H

#include "interface/namespace.h"

#include <QObject>
#include <QMap>

#include "user.h"

namespace DCC_NAMESPACE {

class UserModel : public QObject
{
    Q_OBJECT
public:
    explicit UserModel(QObject *parent = nullptr);
    ~UserModel();

    User *getUser(const QString &id);
    QList<User *> userList() const;
    void addUser(const QString &id, User *user);
    void removeUser(const QString &id);
    bool contains(const QString &id);

    inline bool isAutoLoginVisable() const { return m_autoLoginVisable; }
    void setAutoLoginVisable(const bool visable);

    inline bool isCreateUserValid() const { return m_bCreateUserValid; }
    void setCreateUserValid(bool bValid);

    inline bool isNoPassWordLoginVisable() const { return m_noPassWordLoginVisable; }
    void setNoPassWordLoginVisable(const bool visable);
    bool isJoinADDomain() const { return m_isJoinADDomain; }
    void setIsJoinADDomain(bool isJoinADDomain);

    bool isADUserLogind() const { return m_isADUserLogind; }
    void setADUserLogind(bool isADUserLogind);
    void setAllGroups(const QStringList &groups);
    QStringList getAllGroups();
    void setPresetGroups(const QStringList &presetGroups);
    QStringList getPresetGroups();
    QString getCurrentUserName() const;
    void setCurrentUserName(const QString &currentUserName);

    bool getIsSecurityHighLever() const;
    void setIsSecurityHighLever(bool isSecurityHighLever);

    inline QStringList getOnlineUsers() { return m_onlineUsers; }
    void SetOnlineUsers(QStringList onlineUsers);

    enum ActionOption {
        ClickCancel = 0,
        CreateUserSuccess,
        ModifyPwdSuccess
    };

Q_SIGNALS:
    void userAdded(User *user);
    void userRemoved(User *user);
    void isJoinADDomainChanged(bool isjoin);
    void isADUserLoginChanged(bool isLogind);
    void allGroupsChange(const QStringList &groups);
    void deleteUserSuccess();
    void autoLoginVisableChanged(bool autoLogin);
    void noPassWordLoginVisableChanged(bool noPassword);
    void isCancelChanged();
    void adminCntChange(const int adminCnt);
private:
    bool m_autoLoginVisable;
    bool m_noPassWordLoginVisable;
    bool m_bCreateUserValid;
    QMap<QString, User *> m_userList;
    QStringList m_allGroups;
    QStringList m_presetGroups;
    QString m_currentUserName;
    bool m_isJoinADDomain;
    bool m_isADUserLogind;
    bool m_isSecurityHighLever;
    QStringList m_onlineUsers;
};
} // namespace DCC_NAMESPACE

#endif // USERMODEL_H
