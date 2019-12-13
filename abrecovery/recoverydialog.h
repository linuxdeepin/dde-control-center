/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#ifndef RECOVERYDIALOG_H
#define RECOVERYDIALOG_H

#include "backgroundwidget.h"

#include <DDialog>

#include <com_deepin_abrecovery.h>
#include <com_deepin_sessionmanager.h>

using AbRecoveryInter = com::deepin::ABRecovery;
using SessionManagerInter = com::deepin::SessionManager;

DWIDGET_USE_NAMESPACE

class RecoveryDialog : public DDialog
{
    Q_OBJECT
public:
    explicit RecoveryDialog(DDialog *parent = nullptr);
    ~RecoveryDialog() override;

    void backupInfomation(QString version, QString time);
    void grabMouseAndKeyboardFocus(bool bgrab = true);
    void updateRestoringWaitUI();
    void destroyRestoringWaitUI();
    void updateRestoringFailedUI();

Q_SIGNALS:
    void notifyButtonClicked(bool);

private:
    void initUI();
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    QString m_backupVersion;
    QString m_backupTime;
    BackgroundWidget *m_restoreWidget;
};

class Manage : public QObject
{
    Q_OBJECT
public:
    explicit Manage(QObject *parent = nullptr);
    void showDialog();

private:
    void recoveryCanRestore();
    void exitApp(bool isExec = true);
    void requestReboot();
    QString getBackupTime();

private:
    AbRecoveryInter *m_systemRecovery;
    SessionManagerInter *m_sessionManager;
    RecoveryDialog *m_dialog;
};

#endif // RECOVERYDIALOG_H
