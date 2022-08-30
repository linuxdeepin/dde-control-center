// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

    /*!
     * \brief updateRestoringWaitUI 正在进行版本回退中,更新提示信息
     */
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

    /*!
     * \brief Manage
     * 在"控制中心",进行"Install update"前,会先备份,备份成功后再升级,升级完成需要重启;(重启后在启动列表中选择更新的启动项)
     * 然后启动该进程,根据构造函数中的条件逐渐往下判断,都满足则showDialog
     * 弹框后,选择"取消并重启"则恢复到旧的版本 , 选择"确定"则使用新的版本
     * \param parent
     */
    explicit Manage(QObject *parent = nullptr);
    void showDialog();

private:
    void recoveryCanRestore();

    /*!
     * \brief exitApp 退出进程
     * \param isExec
     */
    void exitApp(bool isExec = true);
    void requestReboot();
    QString getBackupTime();

private:
    AbRecoveryInter *m_systemRecovery;
    SessionManagerInter *m_sessionManager;
    RecoveryDialog *m_dialog;
};

#endif // RECOVERYDIALOG_H
