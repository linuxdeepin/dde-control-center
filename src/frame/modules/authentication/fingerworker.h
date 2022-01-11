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

#include <QObject>
#include <com_deepin_daemon_authenticate_fingerprint.h>
#include <com_deepin_sessionmanager.h>

using com::deepin::daemon::authenticate::Fingerprint;
using SessionManagerInter = com::deepin::SessionManager;

namespace dcc {
namespace authentication {

class FingerWorker : public QObject
{
    Q_OBJECT
public:
    explicit FingerWorker(FingerModel *model, QObject *parent = nullptr);

public Q_SLOTS:
    void tryEnroll(const QString &name, const QString &thumb);
    void refreshUserEnrollList(const QString &id);
    void stopEnroll(const QString& userName);
    void deleteFingerItem(const QString& userName, const QString& finger);
    void renameFingerItem(const QString& userName, const QString& finger, const QString& newName);

private:
    FingerModel *m_model;
    Fingerprint *m_fingerPrintInter;
    SessionManagerInter *m_SMInter;
};

}
}

#endif // FINGERWORKER_H
