/*
* Copyright (C) 2017 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangdongdong <zhangdongdong@uniontech.com>
*
* Maintainer: zhangdongdong <zhangdongdong@uniontech.com>
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

#include "pwqualitymanager.h"

#include <QDebug>
#include <QSettings>
#include <QSet>
#include <QFileInfo>

PwQualityManager::PwQualityManager()
{
}

PwQualityManager *PwQualityManager::instance()
{
    static PwQualityManager pwquality;
    return &pwquality;
}

bool PwQualityManager::containsChar(const QString &password, const QString &validate)
{
    for (const QChar &p : password) {
        if (!validate.contains(p)) {
            return false;
        }
    }

    return true;
}

int PwQualityManager::passwordCompositionType(const QStringList &validate, const QString &password)
{
    return static_cast<int>(std::count_if(validate.cbegin(), validate.cend(),
                                          [=](const QString &policy) {
        for (const QChar &c : policy) {
            if (password.contains(c)) {
                return true;
            }
        }
        return false;
    }));

}

PwQualityManager::ERROR_TYPE PwQualityManager::verifyPassword(const QString &username, const QString &password)
{
    QFileInfo fileInfo(PASSWD_CONF_FILE);
    if (fileInfo.isFile()) {
        // NOTE(justforlxz): 配置文件由安装器生成，后续改成PAM模块
        QSettings setting(PASSWD_CONF_FILE, QSettings::IniFormat);
        setting.beginGroup("Password");
        m_strongPasswordCheck = setting.value("STRONG_PASSWORD", false).toBool();
        m_passwordMinLen = setting.value("PASSWORD_MIN_LENGTH").toInt();
        m_passwordMaxLen = setting.value("PASSWORD_MAX_LENGTH").toInt();
        m_validatePolicy = setting.value("VALIDATE_POLICY").toString().split(";");
        m_validateRequired = setting.value("VALIDATE_REQUIRED").toInt();

        QString reversusername;
        QStringList reversenamelist;

        for (int i = username.count() - 1; i > -1; i--) {
            reversenamelist << username.at(i) ;
        }
        reversusername = reversenamelist.join("");

        if (!m_strongPasswordCheck) {
            return ENUM_PASSWORD_NOERROR ;
        }

        if (password.size() == 0) {
            return ENUM_PASSWORD_EMPTY;
        }

        if (password.size() < m_passwordMinLen) {
            return ENUM_PASSWORD_TOOSHORT ;
        }

        if (password.size() > m_passwordMaxLen) {
            return ENUM_PASSWORD_TOOLONG ;
        }

        //由于安装器之前配置文件中,反斜杠被当成转义字符了,这里确保校验规则中包含以下字符: "\", "|", "空格符"
        for (auto &policy : m_validatePolicy) {
            if (policy.contains("~")) {
                if (!policy.contains("\\")) {
                    policy.append("\\");
                }
                if (!policy.contains("|")) {
                    policy.append("|");
                }
                if (!policy.contains(" ")) {
                    policy.append(" ");
                }
            }
        }

        // 密码不能包含校验规则字符之外的其他字符
        QString allPolicyStr;
        for (auto policy : m_validatePolicy) {
            allPolicyStr.append(policy);
        }

        // 根据最新需求密码规则有效字符不包含空格
        allPolicyStr = allPolicyStr.replace(" ","");

        if (!containsChar(password, allPolicyStr)) {
            return ENUM_PASSWORD_CHARACTER;
        }

        // 密码至少同时包括小写字母,大写字母,数字,符号中的m_validate_Required种
        if (passwordCompositionType(m_validatePolicy, password) < m_validateRequired) {
            return ENUM_PASSWORD_SEVERAL;
        }

        // 密码不可为用户名重复或倒置
        reversusername = reversenamelist.join("");
        if (password == username || password == reversusername) {
            return ENUM_PASSWORD_REPEATED;
        }
    } else {
        QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<>");
        bool ret = containsChar(password, validate_policy);
        if (!ret) {
            return ENUM_PASSWORD_CHARACTER;
        }
    }

    return ENUM_PASSWORD_SUCCESS;
}

QString PwQualityManager::getErrorTips(PwQualityManager::ERROR_TYPE types)
{
    switch (types) {
    case ENUM_PASSWORD_EMPTY:
        return tr("Password cannot be empty");
    case ENUM_PASSWORD_TOOSHORT:
        return tr("Password must be between %1 and %2 characters").arg(m_passwordMinLen).arg(m_passwordMaxLen);
    case ENUM_PASSWORD_SEVERAL:
        return tr("The password must have at least %1 characters, and contain at least %2 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_passwordMinLen).arg(m_validateRequired);
    case ENUM_PASSWORD_TOOLONG:
        return tr("Password must be no more than %1 characters").arg(m_passwordMaxLen);
    case ENUM_PASSWORD_TYPE:
        return tr("The password should contain at least %1 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_validateRequired);
    case ENUM_PASSWORD_CHARACTER:
        return tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)");
    case ENUM_PASSWORD_REPEATED:
        return tr("Password should not be the repeated or reversed username");
    default:
        qDebug() << Q_FUNC_INFO << "error return, please check error types";
        return "";
    }
}

