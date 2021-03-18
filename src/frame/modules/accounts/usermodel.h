/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
private:
    bool m_autoLoginVisable;
    bool m_noPassWordLoginVisable;
    bool m_bCreateUserValid;
    QMap<QString, User *> m_userList;
    QStringList m_allGroups;
    QStringList m_presetGroups;
#ifdef DCC_ENABLE_ADDOMAIN
    bool m_isJoinADDomain;
    bool m_isADUserLogind;
#endif
};

} // namespace accounts
} // namespace dcc

#endif // USERMODEL_H
