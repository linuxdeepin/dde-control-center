// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "usermodel.h"

#include <QDebug>

using namespace dcc::accounts;

UserModel::UserModel(QObject *parent)
    : QObject(parent)
    , m_autoLoginVisable(true)
    , m_noPassWordLoginVisable(true)
    , m_bCreateUserValid(false)
#ifdef DCC_ENABLE_ADDOMAIN
    , m_isJoinADDomain(false)
    , m_isADUserLogind(false)
    , m_isSecurityHighLever(false)
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

void UserModel::setAutoLoginVisable(const bool visable)
{
    if (m_autoLoginVisable == visable)
        return;

    m_autoLoginVisable = visable;
    Q_EMIT autoLoginVisableChanged(m_autoLoginVisable);
}

void UserModel::setCreateUserValid(bool bValid)
{
    if (m_bCreateUserValid == bValid)
        return;

    m_bCreateUserValid = bValid;
}

void UserModel::setNoPassWordLoginVisable(const bool visable)
{
    if (m_noPassWordLoginVisable == visable)
        return;

    m_noPassWordLoginVisable = visable;
    Q_EMIT noPassWordLoginVisableChanged(m_noPassWordLoginVisable);
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
    if (m_allGroups == groups) {
        return;
    }
    m_allGroups = groups;
    Q_EMIT allGroupsChange(groups);
}

QStringList UserModel::getPresetGroups()
{
    return m_presetGroups;
}

QString UserModel::getCurrentUserName() const
{
    return m_currentUserName;
}

void UserModel::setCurrentUserName(const QString &currentUserName)
{
    m_currentUserName = currentUserName;
}

bool UserModel::getIsSecurityHighLever() const
{
    return m_isSecurityHighLever;
}

void UserModel::setIsSecurityHighLever(bool isSecurityHighLever)
{
    m_isSecurityHighLever = isSecurityHighLever;
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
