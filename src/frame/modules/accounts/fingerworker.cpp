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

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QProcess>

using namespace dcc;
using namespace dcc::accounts;

const QString FingerPrintService("com.deepin.daemon.Authenticate");

#define TEST false

FingerWorker::FingerWorker(FingerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_fingerPrintInter(new Fingerprint(FingerPrintService, "/com/deepin/daemon/Authenticate/Fingerprint",
                                         QDBusConnection::systemBus(), this))
{
//    m_fingerPrintInter->setSync(false);

    //处理指纹后端的录入状态信号
    connect(m_fingerPrintInter, &Fingerprint::EnrollStatus, m_model, [this](const QString &userName, int code, const QString &msg) {
        m_model->onEnrollStatusChanged(code, msg);
    });
    //当前此信号末实现
    connect(m_fingerPrintInter, &Fingerprint::Touch, m_model, &FingerModel::onTouch);

    QDBusInterface *inter = new QDBusInterface(FingerPrintService, "/com/deepin/daemon/Authenticate/Fingerprint",
                                               "com.deepin.daemon.Authenticate.Fingerprint",
                                               QDBusConnection::systemBus(), this);
    auto defualtDevice = m_fingerPrintInter->defaultDevice();
    qDebug() << "defaultDevice:" << defualtDevice;
    m_model->setIsVaild(!defualtDevice.isEmpty());
}

bool FingerWorker::tryEnroll(const QString &name, const QString &thumb)
{
    auto callClaim = m_fingerPrintInter->Claim(name, true);
    callClaim.waitForFinished();
    if (callClaim.isError()) {
        qDebug() << "call DeleteFinger Error : " << callClaim.error();
        return false;
    }

    auto callEnroll =  m_fingerPrintInter->Enroll(thumb);
    callEnroll.waitForFinished();
    if (callEnroll.isError()) {
        qDebug() << "call Enroll Error : " << callClaim.error();
        return false;
    }

    return true;
}

void FingerWorker::refreshUserEnrollList(const QString &id)
{
    qDebug() << "refreshUserEnrollList" << "id:" << id;
    auto call = m_fingerPrintInter->ListFingers(id);
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call ListFingers Error : " << call.error();
    } else {
        qDebug() << "ListFingers  : " << call.value();
    }
    m_model->setThumbsList(call.value());
}

void FingerWorker::startEnroll(const QString &name, const QString &thumb)
{
    tryEnroll(name, thumb);
}

void FingerWorker::stopEnroll(const QString& userName)
{
    qDebug() << "stopEnroll";
    auto call = m_fingerPrintInter->StopEnroll();
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call StopEnroll Error : " << call.error();
    }
    auto callClaim = m_fingerPrintInter->Claim(userName, false);
    callClaim.waitForFinished();
    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
    }
}

void FingerWorker::deleteFingerItem(const QString& userName, const QString& finger)
{
    auto call = m_fingerPrintInter->DeleteFinger(userName, finger);
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call DeleteFinger Error : " << call.error();
    }
    refreshUserEnrollList(userName);
}

bool FingerWorker::reRecordFinger(const QString &thumb)
{
    qDebug() << "recordFinger";
    //1, 判断设备是否可用, 如果非独占，返回error
    //2, 调用StopEnroll
    QDBusPendingCall call = m_fingerPrintInter->StopEnroll();
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << "call StopEnroll Error : " << call.error();
        return false;
    }

    call = m_fingerPrintInter->Enroll(thumb);
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << "call Enroll Error : " << call.error();
        return false;
    }

    return true;
}
