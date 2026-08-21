//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

#endif  // DEEPIN_INSTALLER_PWQUALITY_MANAGER_H
