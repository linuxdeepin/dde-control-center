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

#include <QSettings>
#include <QDebug>

using namespace dcc::accounts;

UserModel::UserModel(QObject *parent)
    : QObject(parent)
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


QString UserModel::validatePassword(const QString &password)
{
    // NOTE(justforlxz): 配置文件由安装器生成，后续改成PAM模块
    QSettings setting("/etc/deepin/dde.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    const bool strong_password_check = setting.value("STRONG_PASSWORD", false).toBool();
    const int  password_min_length   = setting.value("PASSWORD_MIN_LENGTH").toInt();
    const int  password_max_length   = setting.value("PASSWORD_MAX_LENGTH").toInt();
    const QStringList validate_policy= setting.value("VALIDATE_POLICY").toString().split(";");
    const int validate_required      = setting.value("VALIDATE_REQUIRED").toInt();

    if (!strong_password_check) {
        return "";
    }

    if (password.size() < password_min_length || password.size() > password_max_length) {
        return QString(tr("Password must be between %1 and %2 characters")
                       ).arg(password_min_length).arg(password_max_length);
    }

    // NOTE(justforlxz): 转换为set，如果密码中包含了不存在与validate_policy中的字符，相减以后不为空。
    if (!(password.split("").toSet() - validate_policy.join("").split("").toSet())
             .isEmpty()) {
        return QString(tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"));
    }

    if (std::count_if(validate_policy.cbegin(), validate_policy.cend(),
                      [=](const QString &policy) {
                          for (const QChar &c : policy) {
                              if (password.contains(c)) {
                                  return true;
                              }
                          }

                          return false;
                      }) < validate_required) {
        return QString(tr("Password must contain at least %1 types of characters")).arg(validate_required);
    }

    return "";
}
