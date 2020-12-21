/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "systeminfowork.h"
#include "systeminfomodel.h"
#include "widgets/basiclistdelegate.h"
#include "dsysinfo.h"

#include <QFutureWatcher>
#include <QtConcurrent>

DCORE_USE_NAMESPACE

namespace dcc{
namespace systeminfo{

static const int ItemWidth = 334;
static const int ItemHeight = 177;

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    :QObject(parent),
      m_model(model)
{
    m_systemInfoInter = new SystemInfoInter("com.deepin.daemon.SystemInfo",
                                            "/com/deepin/daemon/SystemInfo",
                                            QDBusConnection::sessionBus(), this);
    m_systemInfoInter->setSync(false);

    m_systemInfo = new QDBusInterface("com.deepin.system.SystemInfo",
                                      "/com/deepin/system/SystemInfo",
                                      "com.deepin.system.SystemInfo",
                                      QDBusConnection::systemBus(), this);

    m_dbusGrub = new GrubDbus("com.deepin.daemon.Grub2",
                              "/com/deepin/daemon/Grub2",
                              QDBusConnection::systemBus(),
                              this);

    m_dbusGrubTheme = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                        "/com/deepin/daemon/Grub2/Theme",
                                        QDBusConnection::systemBus(), this);

    if (DSysInfo::productType() == DSysInfo::Deepin || DSysInfo::productType() == DSysInfo::Uos) {
        m_activeInfo = new QDBusInterface("com.deepin.license",
                                          "/com/deepin/license/Info",
                                          "com.deepin.license.Info",
                                          QDBusConnection::systemBus(),this);
    }
#if 0
    //预留接口
    m_dbusActivator = new GrubThemeDbus("com.deepin.license",
                                        "/com/deepin/license/Info",
                                        "com.deepin.license.Info", this);
#endif

    QDBusInterface Interface("com.deepin.daemon.SystemInfo",
                             "/com/deepin/daemon/SystemInfo",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("Get", "com.deepin.daemon.SystemInfo", "CPUMaxMHz");
    QList<QVariant> outArgs = reply.arguments();
    double cpuMaxMhz = outArgs.at(0).value<QDBusVariant>().variant().toDouble();
    if (DSysInfo::cpuModelName().contains("Hz")) {
        m_model->setProcessor(DSysInfo::cpuModelName());
    } else {
        if(DSysInfo::cpuModelName().isEmpty()){
            m_model->setProcessor(QString("%1GHz").arg(cpuMaxMhz / 1000));
        } else {
            m_model->setProcessor(QString("%1 @ %2GHz").arg(DSysInfo::cpuModelName())
                                  .arg(cpuMaxMhz / 1000));
        }
    }

    QDBusConnection::sessionBus().connect("com.deepin.daemon.SystemInfo",
                                          "/com/deepin/daemon/SystemInfo",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(processChanged(QDBusMessage)));

    m_dbusGrub->setSync(false, false);
    m_dbusGrubTheme->setSync(false, false);

    if (DSysInfo::productType() == DSysInfo::Deepin || DSysInfo::productType() == DSysInfo::Uos) {
        connect(m_activeInfo, SIGNAL(LicenseStateChange()),this, SLOT(licenseStateChangeSlot()));
    }

    connect(m_dbusGrub, &GrubDbus::DefaultEntryChanged, m_model, &SystemInfoModel::setDefaultEntry);
    connect(m_dbusGrub, &GrubDbus::EnableThemeChanged, m_model, &SystemInfoModel::setThemeEnabled);
    connect(m_dbusGrub, &GrubDbus::TimeoutChanged, this, [this] (const int &value) {
        m_model->setBootDelay(value > 1);
    });
    connect(m_dbusGrub, &__Grub2::UpdatingChanged, m_model, &SystemInfoModel::setUpdating);

    connect(m_dbusGrub, &GrubDbus::serviceStartFinished, this, [=] {
        QTimer::singleShot(100, this, &SystemInfoWork::grubServerFinished);
    }, Qt::QueuedConnection);

    connect(m_dbusGrubTheme, &GrubThemeDbus::BackgroundChanged, this, &SystemInfoWork::onBackgroundChanged);


    connect(m_systemInfoInter, &__SystemInfo::DistroIDChanged, m_model, &SystemInfoModel::setDistroID);
    connect(m_systemInfoInter, &__SystemInfo::DistroVerChanged, m_model, &SystemInfoModel::setDistroVer);
    connect(m_systemInfoInter, &__SystemInfo::DiskCapChanged, m_model, &SystemInfoModel::setDisk);

    QProcess process;
    process.start("uname -r");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int idx = output.indexOf('\n');
    if ( -1 != idx) {
        output.remove(idx, 1);
    }
    m_model->setKernel(output);

    if (DSysInfo::productType() == DSysInfo::Deepin || DSysInfo::productType() == DSysInfo::Uos) {
        licenseStateChangeSlot();
    }
}

void SystemInfoWork::activate()
{
    m_model->setDistroID(m_systemInfoInter->distroID());
    m_model->setDistroVer(m_systemInfoInter->distroVer());
    m_model->setDisk(m_systemInfoInter->diskCap());

    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer ||
            (DSysInfo::uosType() == DSysInfo::UosType::UosDesktop)) {
        QString productName = QString("%1").arg(DSysInfo::uosSystemName());
        m_model->setProductName(productName);
        QString versionNumber = QString("%1").arg(DSysInfo::majorVersion());
        m_model->setVersionNumber(versionNumber);
    }
    QString version;
    if (DSysInfo::isDeepin()) {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName())
                                  .arg(DSysInfo::minorVersion());
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion())
                                  .arg(DSysInfo::productTypeString());
    }

    m_model->setVersion(version);
    m_model->setType(QSysInfo::WordSize);

    if (m_systemInfo->isValid()) {
        m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), m_systemInfo->property("MemorySize").toULongLong());
    } else {
        m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), static_cast<qulonglong>(DSysInfo::memoryInstalledSize()));
    }
}

void SystemInfoWork::deactivate()
{

}

void SystemInfoWork::processChanged(QDBusMessage msg)
{
    QList<QVariant> outArgs = msg.arguments();
    double cpuMaxMhz = outArgs.at(0).value<QDBusVariant>().variant().toDouble();
    if (DSysInfo::cpuModelName().contains("Hz")) {
        m_model->setProcessor(DSysInfo::cpuModelName());
    } else {
        if(DSysInfo::cpuModelName().isEmpty()){
            m_model->setProcessor(QString("%1GHz").arg(cpuMaxMhz / 1000));
        } else {
            m_model->setProcessor(QString("%1 @ %2GHz").arg(DSysInfo::cpuModelName())
                                  .arg(cpuMaxMhz / 1000));
        }
    }
}

void SystemInfoWork::loadGrubSettings()
{
    if (m_dbusGrub->isValid()) {
        grubServerFinished();
    } else {
        m_dbusGrub->startServiceProcess();
    }
}

void SystemInfoWork::setBootDelay(bool value)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetTimeout(value ? 5 : 1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        if (w->isError()) {
            Q_EMIT m_model->bootDelayChanged(m_model->bootDelay());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void SystemInfoWork::setEnableTheme(bool value)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetEnableTheme(value);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        if (w->isError()) {
            Q_EMIT m_model->themeEnabledChanged(m_model->themeEnabled());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void SystemInfoWork::setDefaultEntry(const QString &entry)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetDefaultEntry(entry);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        if (w->isError()) {
            Q_EMIT m_model->defaultEntryChanged(m_model->defaultEntry());
        }

        Q_EMIT requestSetAutoHideDCC(true);
        w->deleteLater();
    });
}

void SystemInfoWork::grubServerFinished()
{
    m_model->setBootDelay(m_dbusGrub->timeout() > 1);
    m_model->setThemeEnabled(m_dbusGrub->enableTheme());
    m_model->setUpdating(m_dbusGrub->updating());

    getEntryTitles();
    onBackgroundChanged();
}

void SystemInfoWork::onBackgroundChanged()
{
    QDBusPendingCall call = m_dbusGrubTheme->GetBackground();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &SystemInfoWork::getBackgroundFinished);
}

void SystemInfoWork::setBackground(const QString &path)
{
    Q_EMIT requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrubTheme->SetBackgroundSourceFile(path);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        Q_EMIT requestSetAutoHideDCC(true);

        if (w->isError()) {
            onBackgroundChanged();
        } else {
            setEnableTheme(true);
        }

        w->deleteLater();
    });
}

void SystemInfoWork::showActivatorDialog()
{
    QDBusInterface activator("com.deepin.license.activator",
                             "/com/deepin/license/activator",
                             "com.deepin.license.activator",
                             QDBusConnection::sessionBus());
    activator.call(QDBus::AutoDetect, "Show");
}

void SystemInfoWork::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher,
            &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &SystemInfoWork::getLicenseState);
    watcher->setFuture(future);
}

void SystemInfoWork::getEntryTitles()
{
    QDBusPendingCall call = m_dbusGrub->GetSimpleEntryTitles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *w) {
        if (!w->isError()) {
            QDBusReply<QStringList> reply = w->reply();
            QStringList entries = reply.value();
            m_model->setEntryLists(entries);
            m_model->setDefaultEntry(m_dbusGrub->defaultEntry());
        } else {
            qDebug() << "get grub entry list failed : " << w->error().message();
        }

        w->deleteLater();
    });
}

void SystemInfoWork::getBackgroundFinished(QDBusPendingCallWatcher *w)
{
    if (!w->isError()) {
        QDBusPendingReply<QString> reply = w->reply();
        const qreal ratio = qApp->devicePixelRatio();

        QPixmap pix = QPixmap(reply.value()).scaled(QSize(static_cast<int>(ItemWidth * ratio), static_cast<int>(ItemHeight * ratio)),
                                                    Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

        const QRect r(0, 0, static_cast<int>(ItemWidth * ratio), static_cast<int>(ItemHeight * ratio));
        const QSize size(static_cast<int>(ItemWidth * ratio), static_cast<int>(ItemHeight * ratio));

        if (pix.width() > static_cast<int>(ItemWidth * ratio) || pix.height() > static_cast<int>(ItemHeight * ratio))
            pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

        pix.setDevicePixelRatio(ratio);

        m_model->setBackground(pix);
    } else {
        qDebug() << w->error().message();
    }

    w->deleteLater();
}

#ifndef DISABLE_ACTIVATOR
void SystemInfoWork::getLicenseState()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qDebug() << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }

    ActiveState reply = licenseInfo.property("AuthorizationState").value<ActiveState>();
    qDebug() << "authorize result:" << reply;
    m_model->setLicenseState(reply);
}
#endif

}
}
