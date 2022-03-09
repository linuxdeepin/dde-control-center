/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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

#ifndef PWQUALITYMANAGER_H
#define PWQUALITYMANAGER_H

#include <DSysInfo>
#include "deepin_pw_check.h"

DCORE_USE_NAMESPACE

const DSysInfo::UosType UosType = DSysInfo::uosType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType); //是否是服务器版

class PwqualityManager : public QObject
{
    Q_OBJECT

public:
    typedef PW_ERROR_TYPE ERROR_TYPE;

    enum CheckType {
        Default,
        Grub2
    };

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
    ERROR_TYPE verifyPassword(const QString &user, const QString &password, CheckType checkType = Default);
    PASSWORD_LEVEL_TYPE GetNewPassWdLevel(const QString &newPasswd);
    QString getErrorTips(ERROR_TYPE type, CheckType checkType = Default);

private:
    PwqualityManager();
    PwqualityManager(const PwqualityManager&) = delete;

    int m_passwordMinLen;
    int m_passwordMaxLen;
};

#endif // REMINDERDDIALOG_H
