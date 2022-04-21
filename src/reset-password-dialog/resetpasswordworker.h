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

#ifndef RESETPASSWORDWORKER_H
#define RESETPASSWORDWORKER_H

#include <com_deepin_daemon_accounts_user.h>
#include <com_deepin_daemon_accounts.h>

using Accounts = com::deepin::daemon::Accounts;
using AccountsUser = com::deepin::daemon::accounts::User;

typedef QMap<int, QString> SecurityQuestionAnswers;

class ResetPasswordWorker : public QObject
{
    Q_OBJECT

public:
    explicit ResetPasswordWorker(const QString& userName, QObject *parent = 0);

Q_SIGNALS:
    void getSecurityQuestionsReplied(const QList<int> securityQuestions);
    void verifySecretQuestionsReplied(const QList<int> securityQuestions);
    void requestBindCheckUbidReplied(const QString& ubid);
    void requestBindCheckReplied(int result);
    void requestVerficationCodeCountReplied(int count);
    void requestVerficationCodeReplied(int result);
    void requestVerifyVerficationCodeReplied(int result);

public Q_SLOTS:
    void getSecurityQuestions();
    void setPasswordHint(const QString &passwordHint);
    void verifySecretQuestions(const QMap<int, QString> &securityQuestions);
    void asyncBindCheck();
    void asyncRequestVerficationCode(const QString &phoneEmail);
    void verifyVerficationCode(const QString &phoneEmail, const QString &code);

private:
    int bindCheck();
    int requestVerficationCode(const QString &phoneEmail);
    int parseError(const QString& errorMsg);

private:
    Accounts *m_accountInter;
    AccountsUser *m_userInter;
    QDBusInterface *m_userQInter;
    QDBusInterface *m_syncHelperInter;
    QString m_ubid;
    QString m_userPath;
};


#endif // RESETPASSWORDWORKER_H
