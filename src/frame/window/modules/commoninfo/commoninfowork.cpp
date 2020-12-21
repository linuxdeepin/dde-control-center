/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commoninfowork.h"
#include "window/mainwindow.h"
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/utils.h"
#include "../../protocolfile.h"

#include "widgets/basiclistdelegate.h"
#include "widgets/utils.h"

#include <signal.h>
#include <QStandardPaths>
#include <QFutureWatcher>
#include <QtConcurrent>

using namespace DCC_NAMESPACE;
using namespace commoninfo;

const QString UeProgramInterface("com.deepin.userexperience.Daemon");
const QString UeProgramObjPath("/com/deepin/userexperience/Daemon");

CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
    , m_title("")
    , m_content("")
{
    m_dBusGrub = new GrubDbus("com.deepin.daemon.Grub2",
                             "/com/deepin/daemon/Grub2",
                             QDBusConnection::systemBus(),
                             this);

    m_dBusGrubTheme = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                       "/com/deepin/daemon/Grub2/Theme",
                                       QDBusConnection::systemBus(), this);

    m_dBusdeepinIdInter = new GrubDevelopMode("com.deepin.deepinid",
                                                "/com/deepin/deepinid",
                                                QDBusConnection::sessionBus(), this);

    if (DSysInfo::productType() == DSysInfo::Deepin || DSysInfo::productType() == DSysInfo::Uos) {
        m_activeInfo = new QDBusInterface("com.deepin.license",
                                          "/com/deepin/license/Info",
                                          "com.deepin.license.Info",
                                          QDBusConnection::systemBus(),this);
        licenseStateChangeSlot();
    }

    m_dBusUeProgram = new UeProgramDbus(UeProgramInterface, UeProgramObjPath, QDBusConnection::systemBus(), this);

    m_commomModel->setIsLogin(m_dBusdeepinIdInter->isLogin());
    m_commomModel->setDeveloperModeState(m_dBusdeepinIdInter->deviceUnlocked());
    m_dBusGrub->setSync(false, false);
    m_dBusGrubTheme->setSync(false, false);

    //监听开发者在线认证失败的错误接口信息
    connect(m_dBusdeepinIdInter, &GrubDevelopMode::Error, this, [](int code, const QString &msg) {
        //系统通知弹窗qdbus 接口
        QDBusInterface  tInterNotify("com.deepin.dde.Notification",
                                                "/com/deepin/dde/Notification",
                                                "com.deepin.dde.Notification",
                                                QDBusConnection::sessionBus());

        //初始化Notify 七个参数
        QString in0("dde-control-center");
        uint in1 = 101;
        QString in2("preferences-system");
        QString in3("");
        QString in4("");
        QStringList in5;
        QVariantMap in6;
        int in7 = 5000;

        //截取error接口 1001:未导入证书 1002:未登录 1003:无法获取硬件信息 1004:网络异常 1005:证书加载失败 1006:签名验证失败 1007:文件保存失败
        QString msgcode = msg;
        msgcode = msgcode.split(":").at(0);
        if (msgcode == "1001") {
            in3 = tr("Failed to get root access");
        } else if (msgcode == "1002") {
            in3 = tr("Please sign in to your Union ID first");
        } else if (msgcode == "1003") {
            in3 = tr("Cannot read your PC information");
        } else if (msgcode == "1004") {
            in3 = tr("No network connection");
        } else if (msgcode == "1005") {
            in3 = tr("Certificate loading failed, unable to get root access");
        } else if (msgcode == "1006") {
            in3 = tr("Signature verification failed, unable to get root access");
        } else if (msgcode == "1007") {
            in3 = tr("Failed to get root access");
        }
        //系统通知 认证失败 无法进入开发模式
        tInterNotify.call("Notify", in0, in1, in2, in3, in4, in5, in6, in7);
    });
    connect(m_dBusdeepinIdInter, &GrubDevelopMode::IsLoginChanged, m_commomModel, &CommonInfoModel::setIsLogin);
    connect(m_dBusdeepinIdInter, &GrubDevelopMode::DeviceUnlockedChanged, m_commomModel, &CommonInfoModel::setDeveloperModeState);
    connect(m_dBusGrub, &GrubDbus::DefaultEntryChanged, m_commomModel, &CommonInfoModel::setDefaultEntry);
    connect(m_dBusGrub, &GrubDbus::EnableThemeChanged, m_commomModel, &CommonInfoModel::setThemeEnabled);
    connect(m_dBusGrub, &GrubDbus::TimeoutChanged, this, [this](const int &value) {
        qDebug()<<" CommonInfoWork::TimeoutChanged  value =  "<< value;
        m_commomModel->setBootDelay(value > 1);
    });
    connect(m_dBusGrub, &__Grub2::UpdatingChanged, m_commomModel, &CommonInfoModel::setUpdating);

    connect(m_dBusGrub, &GrubDbus::serviceStartFinished, this, [ = ] {
        QTimer::singleShot(100, this, &CommonInfoWork::grubServerFinished);
    }, Qt::QueuedConnection);

    connect(m_dBusGrubTheme, &GrubThemeDbus::BackgroundChanged, this, &CommonInfoWork::onBackgroundChanged);

    if (DSysInfo::productType() == DSysInfo::Deepin || DSysInfo::productType() == DSysInfo::Uos) {
        connect(m_activeInfo, SIGNAL(LicenseStateChange()),this, SLOT(licenseStateChangeSlot()));
    }
}

CommonInfoWork::~CommonInfoWork()
{
    qDebug() << "~CommonInfoWork";
    if (m_process) {
        //如果控制中心被强制关闭，需要用kill来杀掉没有被关闭的窗口
        kill(m_process->pid(), 15);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void CommonInfoWork::activate()
{
}

void CommonInfoWork::deactivate()
{
}

void CommonInfoWork::loadGrubSettings()
{
    if (m_dBusGrub->isValid()) {
        grubServerFinished();
    } else {
        m_dBusGrub->startServiceProcess();
    }
}

bool CommonInfoWork::defaultUeProgram()
{
    return m_dBusUeProgram->IsEnabled();
}

void CommonInfoWork::setBootDelay(bool value)
{
    qDebug()<<" CommonInfoWork::setBootDelay  value =  "<< value;
    QDBusPendingCall call = m_dBusGrub->SetTimeout(value ? 5 : 1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->bootDelayChanged(m_commomModel->bootDelay());
        }

        w->deleteLater();
    });
}

void CommonInfoWork::setEnableTheme(bool value)
{
    QDBusPendingCall call = m_dBusGrub->SetEnableTheme(value);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->themeEnabledChanged(m_commomModel->themeEnabled());
        }

        w->deleteLater();
    });
}

void CommonInfoWork::setDefaultEntry(const QString &entry)
{
    QDBusPendingCall call = m_dBusGrub->SetDefaultEntry(entry);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (!w->isError()) {
            Q_EMIT m_commomModel->defaultEntryChanged(m_dBusGrub->defaultEntry());
        }

        w->deleteLater();
    });
}

void CommonInfoWork::grubServerFinished()
{
    m_commomModel->setBootDelay(m_dBusGrub->timeout() > 1);
    m_commomModel->setThemeEnabled(m_dBusGrub->enableTheme());
    m_commomModel->setUpdating(m_dBusGrub->updating());

    getEntryTitles();
    onBackgroundChanged();
}

void CommonInfoWork::onBackgroundChanged()
{
    QDBusPendingCall call = m_dBusGrubTheme->GetBackground();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &CommonInfoWork::getBackgroundFinished);
}

void CommonInfoWork::setBackground(const QString &path)
{
    QDBusPendingCall call = m_dBusGrubTheme->SetBackgroundSourceFile(path);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            onBackgroundChanged();
        } else {
            setEnableTheme(true);
        }

        w->deleteLater();
    });
}

void CommonInfoWork::setUeProgram(bool enabled, DCC_NAMESPACE::MainWindow *pMainWindow)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    if (enabled && (m_dBusUeProgram->IsEnabled() != enabled)) {
        qInfo("suser opened experience project switch.");
        // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
        QString allowContent(tr("Agree and Join User Experience Program"));

        // license路径
        m_content = ProtocolFile::getUserExpContent();

        m_process = new QProcess(this);

        auto pathType = "-c";
        QStringList sl;
        sl << "zh_CN" << "zh_TW";
        if (!sl.contains(QLocale::system().name()))
            pathType = "-e";
        m_process->start("dde-license-dialog",
                                      QStringList() << "-t" << m_title << pathType << m_content << "-a" << allowContent);
        qDebug()<<" Deliver content QStringList() = "<<"dde-license-dialog"
                                                     << "-t" << m_title << pathType << m_content << "-a" << allowContent;
        connect(m_process, &QProcess::stateChanged, this, [pMainWindow](QProcess::ProcessState state) {
            if (pMainWindow) {
                pMainWindow->setEnabled(state != QProcess::Running);
            } else {
                qDebug() << "setUeProgram pMainWindow is nullptr";
            }
        });
        connect(m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
            if (96 == result) {
                if (!m_commomModel->ueProgram()) {
                    m_commomModel->setUeProgram(enabled);
                    qInfo() << QString("On %1, users open the switch to join the user experience program!").arg(current_date);
                }
                m_dBusUeProgram->Enable(enabled);
            } else {
                m_commomModel->setUeProgram(m_dBusUeProgram->IsEnabled());
                qInfo() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
            }
            m_process->deleteLater();
            m_process = nullptr;
        });
    } else {
        if (m_dBusUeProgram->IsEnabled() != enabled) {
            m_dBusUeProgram->Enable(enabled);
            qDebug() << QString("On %1, users close the switch to join the user experience program!").arg(current_date);
        }
        if (m_commomModel->ueProgram() != enabled) {
            m_commomModel->setUeProgram(enabled);
            qDebug() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
        }
    }
}

void CommonInfoWork::setEnableDeveloperMode(bool enabled, DCC_NAMESPACE::MainWindow *pMainWindow)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");

    if (!enabled)
        return;

    m_dBusdeepinIdInter->setSync(false);
    // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
    QString title(tr("The Disclaimer of Developer Mode"));
    QString allowContent(tr("Agree and Request Root Access"));

    // license内容
    QString content = getDevelopModeLicense(":/systeminfo/license/deepin-end-user-license-agreement_developer_community_%1.txt", "");
    QString contentPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append("tmpDeveloperMode.txt");// 临时存储路径
    QFile *contentFile = new QFile(contentPath);
    // 如果文件不存在，则创建文件
    if (!contentFile->exists()) {
        contentFile->open(QIODevice::WriteOnly);
        contentFile->close();
    }
    // 写入文件内容
    if (!contentFile->open(QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return;
    contentFile->write(content.toLocal8Bit());
    contentFile->close();

    auto pathType = "-c";
    QStringList sl;
    sl << "zh_CN" << "zh_TW";
    if (!sl.contains(QLocale::system().name()))
        pathType = "-e";

    m_process = new QProcess(this);
    m_process->start("dde-license-dialog", QStringList() << "-t" << title << pathType << contentPath << "-a" << allowContent);
    connect(m_process, &QProcess::stateChanged, this, [pMainWindow](QProcess::ProcessState state) {
        if (pMainWindow) {
            pMainWindow->setEnabled(state != QProcess::Running);
        } else {
            qDebug() << "setEnableDeveloperMode pMainWindow is nullptr";
        }
    });

    connect(m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            m_dBusdeepinIdInter->call("UnlockDevice");
        } else {
            qInfo() << QString("On %1, Remove developer mode Disclaimer!").arg(current_date);
        }
        contentFile->remove();
        contentFile->deleteLater();
        m_process->deleteLater();
        m_process = nullptr;
    });
}

void CommonInfoWork::login()
{
    Q_ASSERT(m_dBusdeepinIdInter);
    m_dBusdeepinIdInter->setSync(true);
    QDBusPendingCall call = m_dBusdeepinIdInter->Login();
    call.waitForFinished();
}

void CommonInfoWork::getEntryTitles()
{
    QDBusPendingCall call = m_dBusGrub->GetSimpleEntryTitles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (!w->isError()) {
            QDBusReply<QStringList> reply = w->reply();
            QStringList entries = reply.value();
            m_commomModel->setEntryLists(entries);
            m_commomModel->setDefaultEntry(m_dBusGrub->defaultEntry());
        } else {
            qDebug() << "get grub entry list failed : " << w->error().message();
        }

        w->deleteLater();
    });
}



void CommonInfoWork::getBackgroundFinished(QDBusPendingCallWatcher *w)
{
    if (!w->isError()) {
        QDBusPendingReply<QString> reply = w->reply();
#if 1
        QPixmap pix = QPixmap(reply.value());
        m_commomModel->setBackground(pix);
#else
        const qreal ratio = qApp->devicePixelRatio();
        QPixmap pix = QPixmap(reply.value()).scaled(QSize(ItemWidth * ratio, ItemHeight * ratio),
                                                    Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

        const QRect r(0, 0, ItemWidth * ratio, ItemHeight * ratio);
        const QSize size(ItemWidth * ratio, ItemHeight * ratio);

        if (pix.width() > ItemWidth * ratio || pix.height() > ItemHeight * ratio)
            pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

        pix.setDevicePixelRatio(ratio);
        m_commomModel->setBackground(pix);
#endif
    } else {
        qDebug() << w->error().message();
    }

    w->deleteLater();
}

void CommonInfoWork::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &CommonInfoWork::getLicenseState);
    watcher->setFuture(future);
}

void CommonInfoWork::getLicenseState()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }

    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "authorize result:" << reply;
    m_commomModel->setActivation(reply == 1 || reply == 3);
}
