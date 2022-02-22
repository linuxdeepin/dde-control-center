/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "fingerworker.h"

#include <QDBusPendingCall>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QProcess>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>

using namespace dcc;
using namespace dcc::authentication;

const QString FingerPrintService("com.deepin.daemon.Authenticate");

FingerWorker::FingerWorker(FingerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_fingerPrintInter(new Fingerprint(FingerPrintService, "/com/deepin/daemon/Authenticate/Fingerprint",
                                         QDBusConnection::systemBus(), this))
    , m_SMInter(new SessionManagerInter("com.deepin.SessionManager", "/com/deepin/SessionManager", QDBusConnection::sessionBus(), this))
{
    struct passwd *pws;
    QString userId;
    pws = getpwuid(getuid());
    userId = QString(pws->pw_name);

    //处理指纹后端的录入状态信号
    connect(m_fingerPrintInter, &Fingerprint::EnrollStatus, m_model, [this](const QString &, int code, const QString &msg) {
        m_model->onEnrollStatusChanged(code, msg);
    });
    //当前此信号末实现
    connect(m_fingerPrintInter, &Fingerprint::Touch, m_model, &FingerModel::onTouch);
    connect(m_SMInter, &SessionManagerInter::LockedChanged, m_model, &FingerModel::lockedChanged);

    auto defualtDevice = m_fingerPrintInter->defaultDevice();
    m_model->setIsVaild(!defualtDevice.isEmpty());
    m_model->setUserName(userId);

    if (!defualtDevice.isEmpty()) {
        refreshUserEnrollList(userId);
    }
}

void FingerWorker::tryEnroll(const QString &name, const QString &thumb)
{
    Q_EMIT requestMainWindowEnabled(false);
    m_fingerPrintInter->setTimeout(1000 * 60 * 60);
    auto callClaim = m_fingerPrintInter->Claim(name, true);
    callClaim.waitForFinished();

    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
        m_model->refreshEnrollResult(FingerModel::EnrollResult::Enroll_ClaimFailed);
    } else {
        m_fingerPrintInter->setTimeout(-1);
        auto callEnroll =  m_fingerPrintInter->Enroll(thumb);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(callEnroll, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (callEnroll.isError()) {
                qDebug() << "call Enroll Error : " << callEnroll.error();
                m_fingerPrintInter->Claim(name, false);
                m_model->refreshEnrollResult(FingerModel::EnrollResult::Enroll_Failed);
            } else {
                m_model->refreshEnrollResult(FingerModel::EnrollResult::Enroll_Success);
            }
            Q_EMIT requestMainWindowEnabled(true);
            watcher->deleteLater();
        });
    }
    m_fingerPrintInter->setTimeout(-1);
}

void FingerWorker::refreshUserEnrollList(const QString &id)
{
    auto call = m_fingerPrintInter->ListFingers(id);
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "m_fingerPrintInter->ListFingers call Error";
        m_model->setThumbsList(QStringList());
        return;
    } else {
        qDebug() << "m_fingerPrintInter->ListFingers";
    }
    m_model->setThumbsList(call.value());
}

void FingerWorker::stopEnroll(const QString& userName)
{
    qDebug() << "stopEnroll";
    auto call = m_fingerPrintInter->StopEnroll();
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call StopEnroll Error" << call.error();
    }
    auto callClaim = m_fingerPrintInter->Claim(userName, false);
    callClaim.waitForFinished();
    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
    }
}

void FingerWorker::deleteFingerItem(const QString& userName, const QString& finger)
{
    Q_EMIT requestMainWindowEnabled(false);
    auto call = m_fingerPrintInter->DeleteFinger(userName, finger);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [ this, userName ]() {
        refreshUserEnrollList(userName);
        Q_EMIT requestMainWindowEnabled(true);
        sender()->deleteLater();
    });
}

void FingerWorker::renameFingerItem(const QString& userName, const QString& finger, const QString& newName)
{
    auto call = m_fingerPrintInter->RenameFinger(userName, finger, newName);
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call RenameFinger Error : " << call.error();
    }
    refreshUserEnrollList(userName);
}
