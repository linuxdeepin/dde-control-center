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
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/utils.h"

#include "widgets/basiclistdelegate.h"
#include "widgets/utils.h"

#include <signal.h>

using namespace DCC_NAMESPACE;
using namespace commoninfo;

const QString UeProgramInterface("com.deepin.userexperience.Daemon");
const QString UeProgramObjPath("/com/deepin/userexperience/Daemon");

CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
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


    m_dBusUeProgram = new UeProgramDbus(UeProgramInterface, UeProgramObjPath, QDBusConnection::systemBus(), this);

    m_commomModel->setIsLogin(m_dBusdeepinIdInter->isLogin());
    m_commomModel->setDeveloperModeState(m_dBusdeepinIdInter->deviceUnlocked());
    m_dBusGrub->setSync(false, false);
    m_dBusGrubTheme->setSync(false, false);

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

void CommonInfoWork::setUeProgram(bool enabled)
{
    if (enabled && (m_dBusUeProgram->IsEnabled() != enabled)) {
        qInfo("suser opened experience project switch.");
        // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
        QString title(tr("UOS Privacy Policy"));
        QString allowContent(tr("Agree and Join User Experience Program"));

        // license内容
        QString content = getLicense(":/systeminfo/license/deepin-end-user-license-agreement_community_%1.txt", "");
        QString contentPath("/tmp/tempLic.txt"); // 临时存储路径
        QFile file(contentPath);
        // 如果文件不存在，则创建文件
        if (!file.exists()) {
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        // 写入文件内容
        if (!file.open(QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate))
            return;
        file.write(content.toLocal8Bit());
        file.close();

        m_process = new QProcess(this);

        auto pathType = "-c";
        QStringList sl;
        sl << "zh_CN" << "zh_TW";
        if (!sl.contains(QLocale::system().name()))
            pathType = "-e";
        int result = m_process->execute("dde-license-dialog",
                                      QStringList() << "-t" << title << pathType << contentPath << "-a" << allowContent);

        m_process->deleteLater();
        m_process = nullptr;

        if (96 == result) {
            if (!m_commomModel->ueProgram()) {
                m_commomModel->setUeProgram(enabled);          
                qInfo("user agreed experience project.");
            }
            m_dBusUeProgram->Enable(enabled);
        } else {
            m_commomModel->setUeProgram(m_dBusUeProgram->IsEnabled());
            qInfo("user closed experience project switch");
        }
        file.remove();
    } else {
        if (m_dBusUeProgram->IsEnabled() != enabled) {
            m_dBusUeProgram->Enable(enabled);
            qInfo("user closed experience project switch.");
        }
        if (m_commomModel->ueProgram() != enabled) {
            m_commomModel->setUeProgram(enabled);
            qInfo("user cancelled experience project.");
        }
    }
}

void CommonInfoWork::setEnableDeveloperMode(bool enabled)
{
    if (!enabled)
        return;

    m_dBusdeepinIdInter->setSync(false);
    // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
    QString title(tr("The Disclaimer of Developer Mode"));
    QString allowContent(tr("Agree and Request Root Access"));

    // license内容
    QString content = getLicense(":/systeminfo/license/deepin-end-user-license-agreement_developer_community_%1.txt", "");
    QString contentPath("/tmp/tmpDeveloperMode.txt"); // 临时存储路径
    QFile file(contentPath);
    // 如果文件不存在，则创建文件
    if (!file.exists()) {
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    // 写入文件内容
    if (!file.open(QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return;
    file.write(content.toLocal8Bit());
    file.close();

    auto pathType = "-c";
    QStringList sl;
    sl << "zh_CN" << "zh_TW";
    if (!sl.contains(QLocale::system().name()))
        pathType = "-e";

    m_process = new QProcess(this);
    int result = m_process->execute("dde-license-dialog",
                                    QStringList() << "-t" << title << pathType << contentPath << "-a" << allowContent);
    m_process->deleteLater();
    m_process = nullptr;

    if(96 == result) {
        QDBusPendingCall call = m_dBusdeepinIdInter->UnlockDevice();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
            if (w->isError()) {
                Q_EMIT m_commomModel->developerModeStateChanged(false);
            }else {
                Q_EMIT m_commomModel->developerModeStateChanged(true);
            }
            w->deleteLater();
        });
    } else {
        Q_EMIT m_commomModel->developerModeStateChanged(false);
    }
    file.remove();
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
            qWarning() << "get grub entry list failed : " << w->error().message();
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
        qWarning() << w->error().message();
    }

    w->deleteLater();
}

