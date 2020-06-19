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

#include <com_deepin_daemon_authenticate_fingerprint.h>
#include <com_deepin_sessionmanager.h>

#include <QObject>

using com::deepin::daemon::authenticate::Fingerprint;
using SessionManagerInter = com::deepin::SessionManager;

namespace dcc {
namespace accounts {

class FingerWorker : public QObject
{
    Q_OBJECT
public:
    enum EnrollResult {
        Enroll_AuthFailed,
        Enroll_ClaimFailed,
        Enroll_Failed,
        Enroll_Success,
        Count
    };
    explicit FingerWorker(FingerModel *model, QObject *parent = nullptr);

    void refreshDevice();

Q_SIGNALS:
    void requestShowAddThumb(const QString &name, const QString &thumb);
    void tryEnrollResult(EnrollResult enrollRes);

public:
    void tryEnroll(const QString &name, const QString &thumb);

public Q_SLOTS:
    void refreshUserEnrollList(const QString &id);
    void stopEnroll(const QString& userName);
    void deleteFingerItem(const QString& userName, const QString& finger);
    void renameFingerItem(const QString& userName, const QString& finger, const QString& newName);

private:
    FingerModel *m_model;
    Fingerprint *m_fingerPrintInter;
    SessionManagerInter *m_SMInter{nullptr};
};

}
}

#endif // FINGERWORKER_H
