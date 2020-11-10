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

#include "user.h"

using namespace dcc::accounts;

User::User(QObject *parent)
    : QObject(parent)
    , m_isCurrentUser(false)
    , m_autoLogin(false)
    , m_online(false)
    , m_nopasswdLogin(false)
    , m_userType(UserType::StandardUser)
    , m_createdTime(0)
{
}

const QString User::name() const
{
    return m_name;
}

void User::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;

        Q_EMIT nameChanged(m_name);
    }
}

void User::setFullname(const QString &fullname)
{
    if (fullname != m_fullname) {
        m_fullname = fullname;

        Q_EMIT fullnameChanged(m_fullname);
    }
}

void User::setAutoLogin(const bool autoLogin)
{
    m_autoLogin = autoLogin;

    Q_EMIT autoLoginChanged(m_autoLogin);
}

void User::setAvatars(const QList<QString> &avatars)
{
    m_avatars = avatars;

    Q_EMIT avatarListChanged(m_avatars);
}

void User::setGroups(const QStringList &groups)
{
    if (m_groups != groups) {
        m_groups = groups;
        Q_EMIT groupsChanged(m_groups);
    }
}

void User::setCurrentAvatar(const QString &avatar)
{
    if (m_currentAvatar != avatar) {
        m_currentAvatar = avatar;

        Q_EMIT currentAvatarChanged(m_currentAvatar);
    }
}

void User::setPassword(const QString &password)
{
    m_password = password;
}

void User::setRepeatPassword(const QString &repeatPassword)
{
    m_repeatPassword = repeatPassword;
}

void User::setOnline(bool online)
{
    if (m_online != online) {
        m_online = online;
        Q_EMIT onlineChanged(online);
    }
}

bool User::nopasswdLogin() const
{
    return m_nopasswdLogin;
}

void User::setNopasswdLogin(bool nopasswdLogin)
{
    if (m_nopasswdLogin == nopasswdLogin)
        return;

    m_nopasswdLogin = nopasswdLogin;

    Q_EMIT nopasswdLoginChanged(nopasswdLogin);
}

const QString User::displayName() const
{
    return m_fullname.isEmpty() ? m_name : m_fullname;
}

void User::setIsCurrentUser(bool isCurrentUser)
{
    if (isCurrentUser == m_isCurrentUser)
        return;

    m_isCurrentUser = isCurrentUser;

    Q_EMIT isCurrentUserChanged(isCurrentUser);
}

void User::setPasswordStatus(const QString& status)
{
    if (m_passwordStatus == status) {
        return;
    }

    m_passwordStatus = status;

    Q_EMIT passwordStatusChanged(status);
}

void User::setCreatedTime(const quint64 & createdtime)
{
    if (m_createdTime == createdtime) {
        return;
    }

    m_createdTime = createdtime;

    Q_EMIT createdTimeChanged(createdtime);
}

void User::setUserType(const int userType)
{
    if (m_userType == userType) {
        return;
    }
    m_userType = userType;
    Q_EMIT userTypeChanged(userType);
}

void User::setIsPasswordExpired(bool isExpired)
{
    if (isExpired == m_isPasswordExpired)
        return;

    m_isPasswordExpired = isExpired;
    Q_EMIT isPasswordExpiredChanged(isExpired);
}

void User::setPasswordAge(const int age)
{
    if (age == m_pwAge)
        return;

    m_pwAge = age;
    Q_EMIT passwordAgeChanged(age);
}

int User::charactertypes(QString password)
{
    int Number_flag = 0;
    int Capital_flag = 0;
    int Small_flag = 0;
    int Symbol_flag = 0;
    QByteArray ba = password.toLatin1();
    const char *s = ba.data();

    while (*s) {
        if ('0' <= *s && '9' >= *s) {
            Number_flag = 1 ;
        } else if ('A' <= *s && 'Z' >= *s) {
            Capital_flag = 1;
        } else if ('a' <= *s && 'z' >= *s) {
            Small_flag = 1;
        } else {
            Symbol_flag = 1;
        }
        s++;
    }
    return Number_flag + Capital_flag + Small_flag + Symbol_flag;
}
