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
#include "window/utils.h"

using namespace DCC_NAMESPACE;
PwqualityManager::PwqualityManager()
    : m_passwordMinLen(0)
    , m_passwordMaxLen(0)
{
}

PwqualityManager *PwqualityManager::instance()
{
    static PwqualityManager pwquality;
    return &pwquality;
}

PwqualityManager::ERROR_TYPE PwqualityManager::verifyPassword(const QString &user, const QString &password, CheckType checkType)
{
    switch (checkType) {
    case PwqualityManager::Default: {
        ERROR_TYPE error = deepin_pw_check(user.toLocal8Bit().data(), password.toLocal8Bit().data(), LEVEL_STRICT_CHECK, nullptr);

        if (error == PW_ERR_PW_REPEAT) {
            error = PW_NO_ERR;
        }
        return error;
    }
    case PwqualityManager::Grub2: {
        // LEVEL_STRICT_CHECK?
        ERROR_TYPE error = deepin_pw_check_grub2(user.toLocal8Bit().data(), password.toLocal8Bit().data(), LEVEL_STANDARD_CHECK, nullptr);

        if (error == PW_ERR_PW_REPEAT) {
            error = PW_NO_ERR;
        }
        return error;
    }
    }

    return PW_NO_ERR;
}

PASSWORD_LEVEL_TYPE PwqualityManager::GetNewPassWdLevel(const QString &newPasswd)
{
    return get_new_passwd_strength_level(newPasswd.toLocal8Bit().data());
}

QString PwqualityManager::getErrorTips(PwqualityManager::ERROR_TYPE type, CheckType checkType)
{
    m_passwordMinLen = (checkType == Default ? get_pw_min_length(LEVEL_STRICT_CHECK) : get_pw_min_length_grub2(LEVEL_STRICT_CHECK));
    m_passwordMaxLen = (checkType == Default ? get_pw_max_length(LEVEL_STRICT_CHECK) : get_pw_max_length_grub2(LEVEL_STRICT_CHECK));

    //通用校验规则
    QMap<int, QString> PasswordFlagsStrMap = {
        {PW_ERR_PASSWORD_EMPTY, tr("Password cannot be empty")},
        {PW_ERR_LENGTH_SHORT, tr("Password must have at least %1 characters").arg(m_passwordMinLen)},
        {PW_ERR_LENGTH_LONG, tr("Password must be no more than %1 characters").arg(m_passwordMaxLen)},
        {PW_ERR_CHARACTER_INVALID, tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~`!@#$%^&*()-_+=|\\{}[]:\"'<>,.?/)")},
    };

    //服务器版校验规则
    if (IsServerSystem) {
        PasswordFlagsStrMap[PW_ERR_CHARACTER_INVALID] = tr("Password must contain uppercase letters, lowercase letters, numbers and symbols (~`!@#$%^&*()-_+=|\\{}[]:\"'<>,.?/)");
        PasswordFlagsStrMap[PW_ERR_PALINDROME] = tr("Password must not contain more than 4 palindrome characters");
        PasswordFlagsStrMap[PW_ERR_WORD] = tr("Do not use common words and combinations as password");
        PasswordFlagsStrMap[PW_ERR_PW_MONOTONE] = tr("Create a strong password please");
        PasswordFlagsStrMap[PW_ERR_PW_CONSECUTIVE_SAME] = tr("Create a strong password please");
        PasswordFlagsStrMap[PW_ERR_PW_FIRST_UPPERM] = tr("Do not use common words and combinations as password");
    }

    //规则校验以外的情况统一返回密码不符合安全要求
    if (PasswordFlagsStrMap.value(type).isEmpty()) {
        PasswordFlagsStrMap[type] = tr("It does not meet password rules");
    }

    return PasswordFlagsStrMap.value(type);
}

