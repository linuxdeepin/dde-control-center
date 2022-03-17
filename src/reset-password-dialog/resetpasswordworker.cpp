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

#include "resetpasswordworker.h"
#include <QtConcurrent>
#include <unistd.h>

ResetPasswordWorker::ResetPasswordWorker(const QString& userName, QObject *parent)
    : QObject (parent)
    , m_accountInter(new Accounts("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this))
    , m_syncHelperInter(new QDBusInterface("com.deepin.sync.Helper", "/com/deepin/sync/Helper", "com.deepin.sync.Helper", QDBusConnection::systemBus(), this))
{
    auto reply = m_accountInter->FindUserByName(userName);
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << QString("get user failed:") << reply.error();
    } else {
        m_userInter = new AccountsUser("com.deepin.daemon.Accounts", reply.value(), QDBusConnection::systemBus(), this);
        m_userQInter = new QDBusInterface("com.deepin.daemon.Accounts", reply.value(),"com.deepin.daemon.Accounts.User", QDBusConnection::systemBus(), this);
    }
    m_userPath = reply.value();
}

void ResetPasswordWorker::getSecurityQuestions()
{
    QDBusReply<QList<int>> reply = m_userQInter->call("GetSecretQuestions");
    if (reply.isValid()) {
        Q_EMIT getSecurityQuestionsReplied(reply.value());
    }
    if (!reply.error().message().isEmpty()) {
        qWarning() << QString("GetSecretQuestions failed:") << reply.error();
    }
    qDebug() << "security questions" << reply.value();
}

void ResetPasswordWorker::setPasswordHint(const QString &passwordHint)
{
    auto reply = m_userInter->SetPasswordHint(passwordHint);
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << QString("setPasswordHint failed:") << reply.error();
        return ;
    }
}

void ResetPasswordWorker::verifySecretQuestions(const QMap<int, QString> &securityQuestions)
{
    QDBusReply<QList<int>> reply = m_userQInter->call("VerifySecretQuestions", QVariant::fromValue(securityQuestions));
    if (reply.isValid()) {
        Q_EMIT verifySecretQuestionsReplied(reply.value());
    }
    if (!reply.error().message().isEmpty()) {
        qWarning() << QString("VerifySecretQuestions failed:") << reply.error();
        return;
    }
}

void ResetPasswordWorker::asyncBindCheck()
{
    QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::finished, [this, watcher] {
        Q_EMIT requestBindCheckReplied(watcher->result());
        watcher->deleteLater();
    });
    QFuture<int> future = QtConcurrent::run(this, &ResetPasswordWorker::bindCheck);
    watcher->setFuture(future);
}

void ResetPasswordWorker::asyncRequestVerficationCode(const QString &phoneEmail)
{
    QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::finished, [this, watcher] {
        Q_EMIT requestVerficationCodeReplied(watcher->result());
        watcher->deleteLater();
    });
    QFuture<int> future = QtConcurrent::run(this, &ResetPasswordWorker::requestVerficationCode, phoneEmail);
    watcher->setFuture(future);
}


int ResetPasswordWorker::bindCheck()
{
    QDBusReply<QString> retUOSID = m_syncHelperInter->call("UOSID");
    QString uosid;
    if (retUOSID.error().message().isEmpty()) {
        uosid = retUOSID.value();
        qDebug() << "UOSID success!";
    } else {
        qWarning() << "UOSID failed:" << retUOSID.error().message();
        return -1;
    }

    QDBusInterface userInter("com.deepin.daemon.Accounts",
                             m_userPath,
                             "com.deepin.daemon.Accounts.User",
                             QDBusConnection::systemBus());
    if (!userInter.isValid()) {
        return -1;
    }

    QVariant retUUID = userInter.property("UUID");
    QString uuid = retUUID.toString();

    QDBusReply<QString> retLocalBindCheck= m_syncHelperInter->call("LocalBindCheck", uosid, uuid);
    if (!retLocalBindCheck.error().message().isEmpty()) {
        qWarning() << "isBinded failed:" << retLocalBindCheck.error().message() << uosid << uuid;
        int code = parseError(retLocalBindCheck.error().message());
        return code;
    }
    m_ubid = retLocalBindCheck.value();
    Q_EMIT requestBindCheckUbidReplied(m_ubid);

    return 0;
}

int ResetPasswordWorker::requestVerficationCode(const QString &phoneEmail)
{
    QDBusReply<int> retResetCaptcha = m_syncHelperInter->call("SendResetCaptcha", m_ubid, phoneEmail);
    if (!retResetCaptcha.error().message().isEmpty()) {
        qWarning() << "SendResetCaptcha failed:" << retResetCaptcha.error().message();
        int code = parseError(retResetCaptcha.error().message());
        return code;
    }
    Q_EMIT requestVerficationCodeCountReplied(retResetCaptcha.value());
    qDebug() << "SendResetCaptcha success:" << retResetCaptcha.value();
    return 0;
}

int ResetPasswordWorker::parseError(const QString& errorMsg)
{
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(errorMsg.toLatin1(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        if (doucment.isObject()) {
            QJsonObject object = doucment.object();
            if (object.contains("code")) {
                QJsonValue value = object.value("code");
                if (value.isDouble()) {
                    return value.toInt();
                }
            }
        }
    }
    return -1;
}

void ResetPasswordWorker::verifyVerficationCode(const QString &phoneEmail, const QString &code)
{
    QDBusReply<bool> retVerifyResetCaptcha = m_syncHelperInter->call("VerifyResetCaptcha",
                                                                   m_ubid,
                                                                   phoneEmail,
                                                                   code);
    int ret = -1;
    if (retVerifyResetCaptcha.error().message().isEmpty()) {
        qDebug() << "VerifyResetCaptcha success";
        ret = 0;
    } else {
        qWarning() << "VerifyResetCaptcha failed:" << retVerifyResetCaptcha.error().message();
        ret = parseError(retVerifyResetCaptcha.error().message());
    }

    Q_EMIT requestVerifyVerficationCodeReplied(ret);
}
