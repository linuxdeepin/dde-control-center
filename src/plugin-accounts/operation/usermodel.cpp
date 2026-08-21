//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "usermodel.h"

#include <QDebug>

using namespace dccV25;

UserModel::UserModel(QObject *parent)
    : QObject(parent)
    , m_autoLoginVisable(true)
    , m_quickLoginVisible(true)
    , m_noPassWordLoginVisable(true)
    , m_bCreateUserValid(false)
    , m_isJoinADDomain(false)
    , m_isADUserLogind(false)
    , m_isSecurityHighLever(false)
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
    for(auto user : m_userList) {
        if (m_onlineUsers.contains(user->name()))
            user->setOnline(true);
        else
            user->setOnline(false);
    }
    return m_userList.values();
}

void UserModel::addUser(const QString &id, User *user)
{
    Q_ASSERT(!m_userList.contains(id));

    m_userList[id] = user;
    connect(user, &User::currentAvatarChanged, this, [this, user](const QString &avatar){
        Q_EMIT avatarChanged(user->id(), avatar);
    });
    connect(user, &User::autoLoginChanged, this, [this, user](const bool enable){
        Q_EMIT autoLoginChanged(user->id(), enable);
    });
    connect(user, &User::quickLoginChanged, this, [this, user](const bool enable){
        Q_EMIT quickLoginChanged(user->id(), enable);
    });
    connect(user, &User::nopasswdLoginChanged, this, [this, user](const bool enable){
        Q_EMIT nopasswdLoginChanged(user->id(), enable);
    });
    connect(user, &User::groupsChanged, this, [this, user](const QStringList &groups){
        Q_EMIT groupsChanged(user->id(), groups);
    });
    connect(user, &User::passwordModifyFinished, this, [this, user](const int exitCode, const QString &errorTxt){
        Q_EMIT passwordModifyFinished(user->id(), exitCode, errorTxt);
    });
    connect(user, &User::passwordResetFinished, this, [this, user](const QString &errorTxt){
        Q_EMIT passwordModifyFinished(user->id(), errorTxt.isEmpty() ? 0 : -1, errorTxt);
    });
    connect(user, &User::onlineChanged, this, [this, user](const bool &online){
        Q_EMIT onlineChanged(user->id(), online);
    });
    connect(user, &User::userTypeChanged, this, [this, user](const int userType){
        Q_EMIT userTypeChanged(user->id(), userType);
    });
    connect(user, &User::fullnameChanged, this, [this, user](const QString &fullname){
        Q_EMIT fullnameChanged(user->id(), fullname);
    });
    connect(user, &User::passwordAgeChanged, this, [this, user](const int age){
        Q_EMIT passwordAgeChanged(user->id(), age);
    });

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

void UserModel::setQuickLoginVisible(const bool visible)
{
    if (m_quickLoginVisible == visible)
        return;

    m_quickLoginVisible = visible;
    Q_EMIT quickLoginVisibleChanged(m_quickLoginVisible);
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

User *UserModel::currentUser()
{
    for (auto user : userList()) {
        if (user->name() == m_currentUserName) {
            return user;
        }
    }

    return nullptr;
}

bool UserModel::getIsSecurityHighLever() const
{
    return m_isSecurityHighLever;
}

void UserModel::setIsSecurityHighLever(bool isSecurityHighLever)
{
    m_isSecurityHighLever = isSecurityHighLever;
}

bool UserModel::isDisabledGroup(const QString &groupName)
{
    return m_DisabledGroups.contains(groupName);
}

void UserModel::setDisabledGroups(const QStringList &groups)
{
    m_DisabledGroups = groups;
}

void UserModel::SetOnlineUsers(QStringList onlineUsers)
{
    m_onlineUsers = onlineUsers;
}

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
