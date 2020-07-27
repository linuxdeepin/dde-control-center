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
    int  m_palindromeLength = 4;
};

#endif  // DEEPIN_INSTALLER_PWQUALITY_MANAGER_H
