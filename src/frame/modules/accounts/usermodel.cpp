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

#include "usermodel.h"

#include <QDebug>

using namespace dcc::accounts;

UserModel::UserModel(QObject *parent)
    : QObject(parent)
    , m_bAutoLoginValid(false)
    , m_bNoPassWordLoginValid(false)
    , m_bCreateUserValid(false)
#ifdef DCC_ENABLE_ADDOMAIN
    , m_isJoinADDomain(false)
    , m_isADUserLogind(false)
#endif
{

}

UserModel::~UserModel()
{
    qDeleteAll(m_userList.values());
}

User * UserModel::getUser(const QString &id)
{
    return m_userList.value(id, nullptr);
}

QList<User *> UserModel::userList() const
{
    return m_userList.values();
}

void UserModel::addUser(const QString &id, User *user)
{
    Q_ASSERT(!m_userList.contains(id));

    m_userList[id] = user;

    Q_EMIT userAdded(user);
}

void UserModel::removeUser(const QString &id)
{
    Q_ASSERT(m_userList.contains(id));

    User *user = m_userList[id];
    m_userList.remove(id);

    Q_EMIT userRemoved(user);
}

bool UserModel::contains(const QString &id)
{
    return m_userList.contains(id);
}

void UserModel::setAutoLoginValid(bool bValid)
{
    if (m_bAutoLoginValid == bValid)
        return;

    m_bAutoLoginValid = bValid;
}

void UserModel::setCreateUserValid(bool bValid)
{
    if (m_bCreateUserValid == bValid)
        return;

    m_bCreateUserValid = bValid;
}

void UserModel::setNoPassWordLoginValid(bool bValid)
{
    if (m_bNoPassWordLoginValid == bValid)
        return;

    m_bNoPassWordLoginValid = bValid;
}

QStringList UserModel::getAllGroups()
{
    return m_allGroups;
}

void UserModel::setPresetGroups(const QStringList &presetGroups)
{
    m_presetGroups = presetGroups;
}

void UserModel::setAllGroups(const QStringList &groups)
{
    m_allGroups = groups;
}

QStringList UserModel::getPresetGroups()
{
    return m_presetGroups;
}

#ifdef DCC_ENABLE_ADDOMAIN
void UserModel::setIsJoinADDomain(bool isJoinADDomain)
{
    if (m_isJoinADDomain == isJoinADDomain)
        return;

    m_isJoinADDomain = isJoinADDomain;

    Q_EMIT isJoinADDomainChanged(isJoinADDomain);
}

void UserModel::setADUserLogind(bool isADUserLogind)
{
    if (m_isADUserLogind == isADUserLogind) {
        return;
    }

    m_isADUserLogind = isADUserLogind;

    Q_EMIT isADUserLoginChanged(isADUserLogind);
}
#endif
