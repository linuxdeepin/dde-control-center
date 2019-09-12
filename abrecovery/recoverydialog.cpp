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
#include "recoverydialog.h"

#include <DLabel>
#include <DFontSizeManager>

//在"控制中心",进行"Install update"前,会先备份,备份成功后再升级,升级完成需要重启;(重启后在启动列表中选择更新的启动项)
//然后启动该进程,根据构造函数中的条件逐渐往下判断,都满足则showDialog
//弹框后,选择"取消并重启"则恢复到旧的版本 , 选择"确定"则使用新的版本
Manage::Manage(QObject *parent)
    : QObject(parent)
    , m_systemRecovery(new AbRecoveryInter("com.deepin.ABRecovery",
                                           "/com/deepin/ABRecovery",
                                           QDBusConnection::systemBus(), this))
    , m_sessionManager(new SessionManagerInter("com.deepin.SessionManager",
                                               "/com/deepin/SessionManager",
                                               QDBusConnection::sessionBus(), this))
    , m_dialog(nullptr)
{
    //满足配置条件,再判断是否满足恢复的条件
    if (m_systemRecovery->configValid()) {
        recoveryCanRestore();
    } else {
        //不满足配置条件,退出app
        exitApp();
    }
}

void Manage::showDialog()
{
    if (m_dialog)
        return;

    m_dialog = new RecoveryDialog;
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_dialog->setVisible(true);

    connect(m_dialog, &RecoveryDialog::notifyButtonClicked, m_sessionManager, [ = ](bool state) {
        //能够进入到弹框页面,说明是满足一切版本回退的条件
        //true: 确认 , 要恢复旧版本
        if (state) {
            m_systemRecovery->StartRestore();
        } else {
            //false: 取消, 使用升级后的版本,需要重启
            requestReboot();
        }
    });

    connect(m_systemRecovery, &AbRecoveryInter::JobEnd, this, [ this ](const QString & kind, bool success, const QString & errMsg) {
        if ("restore" != kind)
            return ;

        //恢复成功,打印log
        if (success) {
            qDebug() << "Restore successed.";
        } else {
            //恢复失败,不做处理并退出当前进程
            qWarning() << Q_FUNC_INFO << " , Recovery restore failed. errMsg : " << errMsg;
        }

        exitApp();
    });

}

void Manage::recoveryCanRestore()
{
    QDBusPendingCall call = m_systemRecovery->CanRestore();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<bool> reply = call.reply();
            bool value = reply.value();
            //满足条件,返回 true : showDialog
            if (value) {
                showDialog();
            } else {
                //不满足恢复条件,退出app
                qWarning() << Q_FUNC_INFO << " , CanRestore : false";
                exitApp();
            }
        } else {
            qWarning() << "recovery Restoring error: " << call.error().message();
        }
    });
}

//退出进程
void Manage::exitApp()
{
    qApp->quit();
}

void Manage::requestReboot()
{
    QDBusPendingCall call = m_sessionManager->RequestReboot();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            qDebug() << Q_FUNC_INFO << " reboot success.";
        } else {
            qWarning() << "recovery Restoring error: " << call.error().message();
        }
        exitApp();
    });
}

RecoveryDialog::RecoveryDialog(DDialog *parent)
    : DDialog(parent)
{
    initUI();
}

RecoveryDialog::~RecoveryDialog()
{

}

void RecoveryDialog::initUI()
{
    DLabel *txt = new DLabel(tr("Are you sure you want to roll back to deepin %1 backed up on %2?").arg("V2.0.0").arg(QDate::currentDate().toString()));
    txt->setMinimumHeight(80);
    txt->setWordWrap(true);
    DFontSizeManager::instance()->bind(txt, DFontSizeManager::T3);

    addContent(txt);
    addButton(tr("Cancel and Reboot"), true, DDialog::ButtonRecommend);
    addButton(tr("Confirm"), true, DDialog::ButtonRecommend);

    connect(this, &DDialog::buttonClicked, this, [ = ](int index, const QString & text) {
        Q_UNUSED(text)
        Q_EMIT notifyButtonClicked(index);
    });
}
