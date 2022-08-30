// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef USERMODEL_H
#define USERMODEL_H

#include <QObject>
#include <QMap>

#include "user.h"

namespace dcc {
namespace accounts {

class UserModel : public QObject
{
    Q_OBJECT
public:
    explicit UserModel(QObject *parent = 0);
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
#ifdef DCC_ENABLE_ADDOMAIN
    bool isJoinADDomain() const { return m_isJoinADDomain; }
    void setIsJoinADDomain(bool isJoinADDomain);

    bool isADUserLogind() const { return m_isADUserLogind; }
    void setADUserLogind(bool isADUserLogind);
#endif
    void setAllGroups(const QStringList &groups);
    QStringList getAllGroups();
    void setPresetGroups(const QStringList &presetGroups);
    QStringList getPresetGroups();
    QString getCurrentUserName() const;
    void setCurrentUserName(const QString &currentUserName);

    bool getIsSecurityHighLever() const;
    void setIsSecurityHighLever(bool isSecurityHighLever);

Q_SIGNALS:
    void userAdded(User *user);
    void userRemoved(User *user);
#ifdef DCC_ENABLE_ADDOMAIN
    void isJoinADDomainChanged(bool isjoin);
    void isADUserLoginChanged(bool isLogind);
#endif
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
#ifdef DCC_ENABLE_ADDOMAIN
    bool m_isJoinADDomain;
    bool m_isADUserLogind;
#endif
    bool m_isSecurityHighLever;
};

} // namespace accounts
} // namespace dcc

#endif // USERMODEL_H
