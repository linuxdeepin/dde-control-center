// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

Q_SIGNALS:
    void requestMainWindowEnabled(const bool isEnabled) const;

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
