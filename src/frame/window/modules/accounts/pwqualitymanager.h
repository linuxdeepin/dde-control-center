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

#ifndef DEEPIN_INSTALLER_PWQUALITY_MANAGER_H
#define DEEPIN_INSTALLER_PWQUALITY_MANAGER_H

#include <cstddef>
#include <pwquality.h>
#include <QString>
#include <QScopedPointer>

enum ErrorType {
    ENUM_PASSWORD_NOTEMPTY,
    ENUM_PASSWORD_TOOLONG,
    ENUM_PASSWORD_TOOSHORT,
    ENUM_PASSWORD_TYPE,
    ENUM_PASSWORD_SEVERAL,
    ENUM_PASSWORD_CHARACTER,
    ENUM_PASSWORD_PALINDROME,
    ENUM_PASSWORD_DICT_FORBIDDEN,
    ENUM_PASSWORD_SUCCESS
};

class PwqualityManager {
public:
    /**
    * @brief PwqualityManager::instance 构造一个 单例
    * @return 返回一个静态实例
    */
    static PwqualityManager* instance();

    /**
    * @brief PwqualityManager::palindromeChecked 进行回文检测
    * @param text 带检测的字符串
    * @return 如果检测到回文字符串，返回检测到的回文字符串，反之返回空的QString对象
    */
    QString palindromeChecked(const QString &text);

    /**
    * @brief PwqualityManager::dictChecked 从字典文件查询是否存在参数text
    * @param text 带检测的字符串
    * @return 如果text在字典中存在，返回内容和输入参数text相同，否则，返回空的String对象
    */
    QString dictChecked(const QString &text);

    /**
    * @brief PwqualityManager::verifyPassword 校验密码
    * @param password 带检密码字符串
    * @return 若找到，返回text，反之返回空
    */
    int verifyPassword(const QString &password);

    /**
    * @brief PwqualityManager::passwordCompositionType 校验密码字符类型个数
    * @param validate 密码字符集合
    * @param password 密码字符串
    * @return 返回检测到的字符类型
    */
    int passwordCompositionType(const QStringList &validate, const QString &password);

    /**
    * @brief PwqualityManager::containsChar 校验密码使用的字符是否合法
    * @param password 密码字符串
    * @param validate 所有密码字符集合
    * @return 密码字符合法返回true,反之返回false
    */
    bool containsChar(const QString &password, const QString &validate);

    inline int getPasswordMinLength() const {
        return m_passwordMinLength;
    }

    inline int getPasswordMaxLength() const {
        return m_passwordMaxLength;
    }

    inline int getValidateRequiredString() const {
        return  m_validateRequiredString;
    }

private:
    PwqualityManager();
    PwqualityManager(const PwqualityManager&) = delete;

    void init();

private:
    struct ScopedPointerCustomDeleter{
        static inline void cleanup(pwquality_settings_t *pointer){
            pwquality_free_settings(pointer);
        }
    };

    QScopedPointer<pwquality_settings_t, ScopedPointerCustomDeleter> m_pwqualitySetting;

    // 回文字符长度要求
    int m_palindromeLength = 4;
    int m_passwordMinLength;
    int m_passwordMaxLength;
    // 密码字符类型个数
    int m_validateRequiredString;
};

#endif  // DEEPIN_INSTALLER_PWQUALITY_MANAGER_H
