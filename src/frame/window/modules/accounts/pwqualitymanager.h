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

#include <QObject>
#include <QString>

#include <deepin_pw_check.h>

class PwqualityManager : public QObject
{
Q_OBJECT
public:
    typedef PW_ERROR_TYPE ERROR_TYPE;

    /**
    * @brief PwqualityManager::instance 构造一个 单例
    * @return 返回一个静态实例
    */
    static PwqualityManager* instance();

    /**
    * @brief PwqualityManager::verifyPassword 校验密码
    * @param password 带检密码字符串
    * @return 若找到，返回text，反之返回空
    */
    ERROR_TYPE verifyPassword(const QString &user, const QString &password);
    QString getErrorTips(ERROR_TYPE type);

private:
    PwqualityManager();
    PwqualityManager(const PwqualityManager&) = delete;

    int m_passwordMinLen;
    int m_passwordMaxLen;
    int m_checkLevel;
};

#endif  // DEEPIN_INSTALLER_PWQUALITY_MANAGER_H
