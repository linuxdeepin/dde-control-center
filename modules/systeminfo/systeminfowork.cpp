/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include "basiclistdelegate.h"

namespace dcc{
namespace systeminfo{

static const int ItemWidth = 344;
static const int ItemHeight = 187;

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    :QObject(parent),
      m_model(model)
{
    m_systemInfoInter = new SystemInfoInter("com.deepin.daemon.SystemInfo",
                                            "/com/deepin/daemon/SystemInfo",
                                            QDBusConnection::sessionBus(),this);
    m_systemInfoInter->setSync(false);
#ifndef DCC_DISABLE_GRUB
    m_dbusGrub = new GrubDbus("com.deepin.daemon.Grub2",
                              "/com/deepin/daemon/Grub2",
                              QDBusConnection::systemBus(),
                              this);

    m_dbusGrubTheme = new GrubThemeDbus("com.deepin.daemon.Grub2",
                                        "/com/deepin/daemon/Grub2/Theme",
                                        QDBusConnection::systemBus(), this);

    connect(m_dbusGrub, &GrubDbus::DefaultEntryChanged, m_model, &SystemInfoModel::setDefaultEntry);
    connect(m_dbusGrub, &GrubDbus::EnableThemeChanged, m_model, &SystemInfoModel::setThemeEnabled);
    connect(m_dbusGrub, &GrubDbus::TimeoutChanged, this, [this] (const int &value) {
        m_model->setBootDelay(value > 1);
    });
    connect(m_dbusGrub, &__Grub2::UpdatingChanged, m_model, &SystemInfoModel::setUpdating);
    connect(m_dbusGrub, &GrubDbus::serviceStartFinished, this, &SystemInfoWork::grubServerFinished);
    connect(m_dbusGrubTheme, &GrubThemeDbus::BackgroundChanged, this, &SystemInfoWork::onBackgroundChanged);
#endif

    connect(m_systemInfoInter, &__SystemInfo::DistroIDChanged, m_model, &SystemInfoModel::setDistroID);
    connect(m_systemInfoInter, &__SystemInfo::DistroVerChanged, m_model, &SystemInfoModel::setDistroVer);
    connect(m_systemInfoInter, &__SystemInfo::VersionChanged, m_model, &SystemInfoModel::setVersion);
    connect(m_systemInfoInter, &__SystemInfo::SystemTypeChanged, m_model, &SystemInfoModel::setType);
    connect(m_systemInfoInter, &__SystemInfo::ProcessorChanged, m_model, &SystemInfoModel::setProcessor);
    connect(m_systemInfoInter, &__SystemInfo::MemoryCapChanged, m_model, &SystemInfoModel::setMemory);
    connect(m_systemInfoInter, &__SystemInfo::DiskCapChanged, m_model, &SystemInfoModel::setDisk);
}

void SystemInfoWork::activate()
{
#ifndef DCC_DISABLE_GRUB
    onBackgroundChanged();
#endif

    m_model->setDistroID(m_systemInfoInter->distroID());
    m_model->setDistroVer(m_systemInfoInter->distroVer());
    m_model->setVersion(m_systemInfoInter->version());
    m_model->setType(m_systemInfoInter->systemType());
    m_model->setProcessor(m_systemInfoInter->processor());
    m_model->setMemory(m_systemInfoInter->memoryCap());
    m_model->setDisk(m_systemInfoInter->diskCap());
}

void SystemInfoWork::deactivate()
{

}

#ifndef DCC_DISABLE_GRUB
void SystemInfoWork::loadGrubSettings()
{
    // NOTE(hualet): DO NOT move below lines to the constructor, it will start
    // the service process which will check authentication on very single request,
    // the popups are really annoying sometime.
    m_dbusGrub->setSync(false);

    if (m_dbusGrub->isValid())
        grubServerFinished();
}

void SystemInfoWork::setBootDelay(bool value)
{
    emit requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetTimeout(value ? 5 : 1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            emit m_model->bootDelayChanged(m_model->bootDelay());
        }

        emit requestSetAutoHideDCC(true);
        watcher->deleteLater();
    });
}

void SystemInfoWork::setEnableTheme(bool value)
{
    emit requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetEnableTheme(value);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            emit m_model->themeEnabledChanged(m_model->themeEnabled());
        }

        emit requestSetAutoHideDCC(true);
        watcher->deleteLater();
    });
}

void SystemInfoWork::setDefaultEntry(const QString &entry)
{
    emit requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrub->SetDefaultEntry(entry);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            emit m_model->defaultEntryChanged(m_model->defaultEntry());
        }

        emit requestSetAutoHideDCC(true);
        watcher->deleteLater();
    });
}

void SystemInfoWork::grubServerFinished()
{
    m_model->setBootDelay(m_dbusGrub->timeout() > 1);
    m_model->setThemeEnabled(m_dbusGrub->enableTheme());
    m_model->setUpdating(m_dbusGrub->updating());;
    getEntryTitles();
}

void SystemInfoWork::onBackgroundChanged()
{
    QDBusPendingCall call = m_dbusGrubTheme->GetBackground();
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(call, this);

    connect(w, &QDBusPendingCallWatcher::finished, this, [=] {
        if (!w->isError()) {
            QDBusPendingReply<QString> reply = w->reply();
            const qreal ratio = qApp->devicePixelRatio();

            QPixmap pix = QPixmap(reply.value()).scaled(QSize(ItemWidth * ratio, ItemHeight * ratio),
                                                                                    Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            const QRect r(0, 0, ItemWidth * ratio, ItemHeight * ratio);
            const QSize size(ItemWidth * ratio, ItemHeight * ratio);

            if (pix.width() > ItemWidth * ratio || pix.height() > ItemHeight * ratio)
                pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

            pix.setDevicePixelRatio(ratio);

            m_model->setBackground(pix);
        }
        w->deleteLater();
    });
}

void SystemInfoWork::setBackground(const QString &path)
{
    emit requestSetAutoHideDCC(false);

    QDBusPendingCall call = m_dbusGrubTheme->SetBackgroundSourceFile(path);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            onBackgroundChanged();

            setEnableTheme(true);
        }

        emit requestSetAutoHideDCC(true);
        watcher->deleteLater();
    });
}

void SystemInfoWork::getEntryTitles()
{
    QDBusPendingCall call = m_dbusGrub->GetSimpleEntryTitles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (!call.isError()) {
            QDBusReply<QStringList> reply = call.reply();
            QStringList entries = reply.value();
            m_model->setEntryLists(entries);
            m_model->setDefaultEntry(m_dbusGrub->defaultEntry());
        } else {
            qWarning() << "get grub entry list failed : " << call.error().message();
        }

        watcher->deleteLater();
    });
}
#endif

}
}
