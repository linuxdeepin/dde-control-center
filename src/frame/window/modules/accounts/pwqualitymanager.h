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

//将创建用户时密码校验规则统一到这里进行调用
//（主线上已经把密码校验统一到后端进行，华为项目暂时还保持原来的校验方式）

#ifndef PWQUALITYMANAGER_H
#define PWQUALITYMANAGER_H

#include <QObject>
#include <QString>


#define PASSWD_CONF_FILE "/etc/deepin/dde.conf"

class PwQualityManager : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        ENUM_PASSWORD_NOERROR,
        ENUM_PASSWORD_EMPTY,
        ENUM_PASSWORD_TOOLONG,
        ENUM_PASSWORD_TOOSHORT,
        ENUM_PASSWORD_TYPE,
        ENUM_PASSWORD_SEVERAL,
        ENUM_PASSWORD_CHARACTER,
        ENUM_PASSWORD_REPEATED,
        ENUM_PASSWORD_SUCCESS
    }PW_ERROR_TYPE;
    typedef PW_ERROR_TYPE ERROR_TYPE;

    /**
    * @brief PwqualityManager::instance 构造一个 单例
    * @return 返回一个静态实例
    */
    static PwQualityManager* instance();

    /**
    * @brief PwqualityManager::verifyPassword 校验密码
    * @param password 带检密码字符串
    * @return 若找到，返回text，反之返回空
    */
    ERROR_TYPE verifyPassword(const QString &username, const QString &password);
    QString getErrorTips(ERROR_TYPE types);
private:
    PwQualityManager();
    bool containsChar(const QString &password, const QString &validate);
    int  passwordCompositionType(const QStringList &validate, const QString &password);

    bool m_strongPasswordCheck;
    int m_passwordMinLen;
    int m_passwordMaxLen;
    int m_validateRequired;
    QStringList m_validatePolicy;

};

#endif // PWQUALITYMANAGER_H
