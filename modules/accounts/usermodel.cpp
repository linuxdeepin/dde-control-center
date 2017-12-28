/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
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

UserModel::UserModel(QObject *parent) :
    QObject(parent)
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

    emit userAdded(user);
}

void UserModel::removeUser(const QString &id)
{
    User * user = getUser(id);
    m_userList.remove(id);

    if (user)
        emit userRemoved(user);
}

bool UserModel::contains(const QString &id)
{
    return m_userList.contains(id);
}
#ifdef DCC_ENABLE_ADDOMAIN
void UserModel::setIsJoinADDomain(bool isJoinADDomain)
{
    if (m_isJoinADDomain == isJoinADDomain)
        return;

    m_isJoinADDomain = isJoinADDomain;

    emit isJoinADDomainChanged(isJoinADDomain);
}
#endif
