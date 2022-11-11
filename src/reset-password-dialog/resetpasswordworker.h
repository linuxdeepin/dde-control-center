// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    void notifySecurityKey(QString);

public Q_SLOTS:
    void getSecurityQuestions();
    void getSecurityKey(QString name);
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
