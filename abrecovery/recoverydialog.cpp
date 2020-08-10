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
#include "backgroundwidget.h"

#include <DLabel>
#include <DFontSizeManager>
#include <DSpinner>
#include <DStyle>

#include <QDesktopWidget>
#include <QApplication>
#include <QWindow>
#include <QHBoxLayout>

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
    qDebug() << "abrecovery: Manage construct";
    //满足配置条件,再判断是否满足恢复的条件
    if (m_systemRecovery->configValid()) {
        recoveryCanRestore();
    } else {
        //不满足配置条件,退出app
        exitApp(false);
    }
}

void Manage::showDialog()
{
    if (m_dialog)
        return;

    m_dialog = new RecoveryDialog;
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);
    m_dialog->backupInfomation(m_systemRecovery->backupVersion(), getBackupTime());
    //设置点击按钮不立即关闭当前页面
    m_dialog->setOnButtonClickedClose(false);

    QTimer::singleShot(300, this, [this]() {
        m_dialog->grabMouseAndKeyboardFocus();
    });

    connect(m_dialog, &RecoveryDialog::notifyButtonClicked, m_sessionManager, [ = ](bool state) {
        //能够进入到弹框页面,说明是满足一切版本回退的条件
        //true: 确认 , 要恢复旧版本
        qDebug() << " state : " << state << " restoring() : " << m_systemRecovery->restoring();
        if (state) {
            m_systemRecovery->StartRestore();
            m_dialog->updateRestoringWaitUI();
        } else {
            //false: 取消, 使用升级后的版本,需要重启
            requestReboot();
        }
    });

    connect(m_systemRecovery, &AbRecoveryInter::JobEnd, this, [ this ](const QString &kind, bool success, const QString &errMsg) {
        qDebug() << "AbRecoveryInter::JobEnd kind: " << kind << ",success:" << success << ",errMsg:" << errMsg;
        m_dialog->destroyRestoringWaitUI();
        if ("restore" != kind) {
            qDebug() << "AbRecoveryInter::JobEnd return:" << kind;
            return;
        }

        //恢复成功,打印log
        if (success) {
            qDebug() << "Restore successed. exitApp";
            exitApp();
        } else {
            //恢复失败,不做处理并退出当前进程
            qDebug() << Q_FUNC_INFO << " , Recovery restore failed. errMsg : " << errMsg;
            m_dialog->updateRestoringFailedUI();
        }
    });
    m_dialog->setVisible(true);
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
                qDebug() << Q_FUNC_INFO << " , CanRestore : false";
                exitApp();
            }
        } else {
            qDebug() << "recovery Restoring error: " << call.error().message();
        }
    });
}

QString Manage::getBackupTime()
{
    time_t t = static_cast<time_t>(m_systemRecovery->backupTime());
    QDateTime time = QDateTime::fromSecsSinceEpoch(t);

    return time.toString("yyyy/MM/dd hh:mm:ss");
}

//退出进程
void Manage::exitApp(bool isExec)
{
    if (isExec) {
        qApp->quit();
    } else {
        exit(0);
    }
}

void Manage::requestReboot()
{
    QDBusPendingCall call = m_sessionManager->RequestReboot();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            qDebug() << Q_FUNC_INFO << " reboot success.";
        } else {
            qDebug() << "recovery Restoring error: " << call.error().message();
        }
        exitApp();
    });
}

RecoveryDialog::RecoveryDialog(DDialog *parent)
    : DDialog(parent)
    , m_backupVersion("")
    , m_backupTime("")
    , m_restoreWidget(nullptr)
{
    setCloseButtonVisible(false);

    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //触发关闭按钮需要退出当前进程
    connect(this, &DDialog::closed, [&]{
        exit(0);
    });
}

RecoveryDialog::~RecoveryDialog()
{

}

void RecoveryDialog::backupInfomation(QString version, QString time)
{
    if (m_backupVersion != version) {
        m_backupVersion = version;
    }

    if (m_backupTime != time) {
        m_backupTime = time;
    }

    initUI();
}

void RecoveryDialog::grabMouseAndKeyboardFocus(bool bgrab)
{
    QDesktopWidget *desktop = qApp->desktop();
    move((desktop->width() - width()) / 2, (desktop->height() - height()) / 2);
    grab();

    if (!bgrab)
        return;

    bool bGrabMouseState = windowHandle()->setMouseGrabEnabled(true);
    bool bGrabKeyboardState = windowHandle()->setKeyboardGrabEnabled(true);
    qDebug() << "抓取鼠标焦点(true : 成功)，setMouseGrabEnabled :" << bGrabMouseState;
    qDebug() << "抓取键盘焦点(true : 成功)， d->setKeyboardGrabEnabled :" << bGrabKeyboardState;

    //获取鼠标焦点失败后，重抓，最多重连3次
    if (!bGrabMouseState) {
        QTimer::singleShot(100, this, [this]() {
            for (int i = 0; i < 3; i++) {
                bool state = windowHandle()->setMouseGrabEnabled(true);
                qDebug() << "抓取鼠标焦点(true : 成功)，setMouseGrabEnabled :" << state;
                if (state) {
                    break;
                }
            }
        });
    }

    //获取键盘焦点失败后，重抓，最多重抓3次
    if (!bGrabKeyboardState) {
        QTimer::singleShot(100, this, [this]() {
            for (int i = 0; i < 3; i++) {
                bool state = windowHandle()->setKeyboardGrabEnabled(true);
                qDebug() << "抓取键盘焦点(true : 成功)setKeyboardGrabEnabled :" << state;
                if (state) {
                    break;
                }
            }
        });
    }
}

//正在进行版本回退中,更新提示信息
void RecoveryDialog::updateRestoringWaitUI()
{
    if (isVisible())
        setVisible(false);

    m_restoreWidget = new BackgroundWidget(true);
    m_restoreWidget->show();
}

void RecoveryDialog::destroyRestoringWaitUI()
{
    if (m_restoreWidget) {
        m_restoreWidget->hide();
        m_restoreWidget->deleteLater();
    }
}

void RecoveryDialog::updateRestoringFailedUI()
{
    qDebug() << Q_FUNC_INFO;
    clearContents();
    clearButtons();

    if (!isVisible())
        setVisible(true);

    setCloseButtonVisible(true);

    setIcon(QApplication::style()->standardIcon(DStyle::SP_MessageBoxWarning));

    QWidget *widget = new QWidget(this);
    setMessage(tr("Rollback failed."));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    widget->setLayout(layout);
    addContent(widget);
    addButton(tr("Reboot"), true, DDialog::ButtonNormal);
    setFocus();
    grabMouseAndKeyboardFocus();
}

void RecoveryDialog::initUI()
{
    DLabel *txt = new DLabel(tr("Are you sure you want to roll back to %1 backed up on %2?").arg("uos " + m_backupVersion).arg(m_backupTime));
    txt->setWordWrap(true);
    addContent(txt);
    addButton(tr("Cancel and Reboot"), true, DDialog::ButtonRecommend);
    addButton(tr("Confirm"), true, DDialog::ButtonRecommend);

    connect(this, &DDialog::buttonClicked, this, [ = ](int index, const QString & text) {
        Q_UNUSED(text)
        Q_EMIT notifyButtonClicked(index);
    });
}

void RecoveryDialog::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    return;
}
