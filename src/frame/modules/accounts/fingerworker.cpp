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

const QString FprintService("com.deepin.daemon.Fprintd");
const QString FingerPrintService("com.deepin.daemon.Authenticate.FingerPrint");

#define TEST true

FingerWorker::FingerWorker(FingerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_fprintdInter(new Fprintd(FprintService, "/com/deepin/daemon/Fprintd", QDBusConnection::systemBus(), this))
    , m_fprDefaultInter(nullptr)
    , m_fingerPrintInter(new Fingerprint(FingerPrintService, "/com/deepin/daemon/Authenticate.FingerPrint",
                                         QDBusConnection::systemBus(), this))
{
    m_fprintdInter->setSync(false);

}

void FingerWorker::refreshUserEnrollList(const QString &name)
{
    QList<QString> listFingers = m_fingerPrintInter->ListFingers(name);
    m_model->setThumbsList(name, listFingers);
}

void FingerWorker::enrollStart(const QString &name, const QString &thumb)
{
    if(TEST) {
        refreshUserEnrollList(name);
    } else {
        if (!m_fingerPrintInter->devices().isEmpty()) {
            //后端接口需要传id不是用户名，需要改
            m_fingerPrintInter->Claim(name, true);
            m_fingerPrintInter->Enroll(name, thumb);
            refreshUserEnrollList(name);
        }
    }
    connect(m_fingerPrintInter, &Fingerprint::EnrollStatus, m_model, [this](const QString &id, int code, const QString &msg) {
      m_model->setTestEnrollStatus(code, msg);
    });
    connect(m_model, &FingerModel::enrollSuccessed, this, [this, name] {
        QTimer time;
        time.setInterval(100);
        connect(&time, &QTimer::timeout, this, [this, name] {refreshUserEnrollList(name);});
    });
//    connect(m_fingerPrintInter, &Fingerprint::Touch, m_model, &FingerModel::testEnrollTouch);//目前不提供识别手指抬起功能
}

void FingerWorker::reEnrollStart(const QString &thumb)
{
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher] {
        if (watcher->result()) {
            m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
        }

        watcher->deleteLater();
    });

    QFuture<bool> future = QtConcurrent::run(this, &FingerWorker::reRecordFinger, thumb);
    watcher->setFuture(future);
}

void FingerWorker::cleanEnroll(User *user)
{
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher, user] {
        if (watcher->result()) {
            m_model->cleanUserThumbs(user->name());
            m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
        }

        watcher->deleteLater();
    });

    QFuture<bool> future = QtConcurrent::run(this, &FingerWorker::cleanFinger, user->name());
    watcher->setFuture(future);
}

void FingerWorker::requestShowAddThumbsaveEnroll(const QString &name)
{
    //当指纹输入完成后，点击完成然后刷新列表，不用停止和释放，会在closeEvent事件中操作
    qDebug() << "saveEnroll: " << name;
    refreshUserEnrollList(name);
    m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
}

void FingerWorker::stopEnroll()
{
    QFutureWatcher<void> watcher(this);

    QFuture<void> future = QtConcurrent::run(this, &FingerWorker::releaseEnroll);
    watcher.setFuture(future);
}

void FingerWorker::deleteFingerItem(const QString& userName, const QString& finger)
{
    m_fingerPrintInter->DeleteFinger(userName, finger);
    m_model->deleteFingerItem(userName, finger);
}

void FingerWorker::onGetFprDefaultDevFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QDBusObjectPath> reply = w->reply();

    w->deleteLater();

    const QDBusObjectPath &path = reply.value();

    if (path.path().isEmpty())
        return;

    m_fprDefaultInter = new Device(FprintService, path.path(), QDBusConnection::systemBus(), this);

    m_model->setIsVaild(m_fprDefaultInter->isValid());

    connect(m_fprDefaultInter, &Device::EnrollStatus, this, &FingerWorker::onEnrollStatus);
}

void FingerWorker::onGetListEnrolledFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QStringList> reply = w->reply();

    if (!reply.isError()) {
        FingerModel::UserThumbs u;
        u.username = w->property("user").toString();
        u.userThumbs = reply.value();
        m_model->addUserThumbs(u);
    } else {
        qDebug() << "onGetListEnrolledFinished: " << reply.error();
    }

    w->deleteLater();
}

void FingerWorker::onEnrollStatus(const QString &value, const bool status)
{
    qDebug() << "onEnrollStatus: " << value << "," << status;
    if (value == "enroll-completed" && status == 1) {
        m_model->setEnrollStatus(FingerModel::EnrollStatus::Finished);
        return;
    }

    if (value == "enroll-retry-scan") {
        m_model->setEnrollStatus(FingerModel::EnrollStatus::Retry);
        return;
    }

    m_model->setEnrollStatus(FingerModel::EnrollStatus::Next);
}

bool FingerWorker::recordFinger(const QString &name, const QString &thumb)
{
    QDBusPendingCall call = m_fprDefaultInter->EnrollStop();
    call.waitForFinished();

    call = m_fprDefaultInter->Release();
    call.waitForFinished();

    QDBusInterface inter(FprintService,
                         "/com/deepin/daemon/Fprintd",
                         FprintService,
                         QDBusConnection::systemBus(), this);
    if (!inter.isValid()) {
        return false;
    }

    QDBusReply<void> reply = inter.call("PreAuthEnroll");
    if (!reply.isValid()) {
        return false;
    }

    call = m_fprDefaultInter->Claim(name);
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
        QProcess::execute("sudo systemctl restart fprintd.service");
        return false;
    }

    call = m_fprDefaultInter->EnrollStart(thumb);
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
        return false;
    }

    return true;
}

bool FingerWorker::reRecordFinger(const QString &thumb)
{
    QDBusPendingCall call = m_fprDefaultInter->EnrollStop();
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
        return false;
    }

    call = m_fprDefaultInter->EnrollStart(thumb);
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
        return false;
    }

    return true;
}

void FingerWorker::releaseEnroll()
{
    QDBusPendingCall call = m_fprDefaultInter->EnrollStop();
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
    }

    call = m_fprDefaultInter->Release();
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
    }
}

bool FingerWorker::cleanFinger(const QString &name)
{
    QDBusPendingCall call = m_fprDefaultInter->DeleteEnrolledFingers(name);
    call.waitForFinished();

    if (call.isError()) {
        qDebug() << call.error();
        return false;
    }

    return true;
}

void FingerWorker::onHandleDevicesChanged(const QList<QDBusObjectPath> &value)
{
    if (value.size() == 0) {
        m_model->setIsVaild(false);
        return;
    }

    const QDBusObjectPath &objpath = value.at(0);
    if (objpath.path().isEmpty()) {
        m_model->setIsVaild(false);
        return;
    }

    if (m_fprDefaultInter == nullptr) {
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_fprintdInter->GetDefaultDevice(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &FingerWorker::onGetFprDefaultDevFinished);
    } else {
        m_model->setIsVaild(true);
    }
}
