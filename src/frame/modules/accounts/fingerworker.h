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

#ifndef FINGERWORKER_H
#define FINGERWORKER_H

#include "fingermodel.h"
#include "user.h"

#include <com_deepin_daemon_fprintd.h>
#include <com_deepin_daemon_fprintd_device.h>
#include <com_deepin_daemon_authenticate_fingerprint.h>

#include <QObject>

using com::deepin::daemon::Fprintd;
using com::deepin::daemon::fprintd::Device;
using com::deepin::daemon::authenticate::Fingerprint;


namespace dcc {
namespace accounts {

class FingerWorker : public QObject
{
    Q_OBJECT
public:
    explicit FingerWorker(FingerModel *model, QObject *parent = nullptr);

    void refreshDevice();

Q_SIGNALS:
    void requestShowAddThumb(const QString &name, const QString &thumb);

public Q_SLOTS:
    void refreshUserEnrollList(const QString &name);
    void enrollStart(const QString &name, const QString &thumb);
    void reEnrollStart(const QString &thumb);
    void cleanEnroll(User *user);
    void saveEnroll(const QString &name);
    void stopEnroll();
    void testEnrollStart(const QString &name, const QString &thumb);
    void deleteFingerItem(const QString& userName, const QString& finger);
private Q_SLOTS:
    void onGetFprDefaultDevFinished(QDBusPendingCallWatcher *w);
    void onGetListEnrolledFinished(QDBusPendingCallWatcher *w);
    void onEnrollStatus(const QString &value, const bool status);
    bool recordFinger(const QString &name, const QString &thumb);
    bool reRecordFinger(const QString &thumb);
    void releaseEnroll();
    bool cleanFinger(const QString &name);
    void onHandleDevicesChanged(const QList<QDBusObjectPath> &value);

private:
    FingerModel *m_model;
    Fprintd *m_fprintdInter;
    Device *m_fprDefaultInter;
    Fingerprint *m_fingerPrintInter;
};

}
}

#endif // FINGERWORKER_H
