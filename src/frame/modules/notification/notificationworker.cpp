/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "notificationworker.h"
#include "model/appitemmodel.h"
#include "model/sysitemmodel.h"
#include <QtConcurrent>

const QString Path    = "/com/deepin/dde/Notification";

using namespace dcc;
using namespace dcc::notification;
NotificationWorker::NotificationWorker(NotificationModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_dbus(new Notification(Notification::staticInterfaceName(), Path, QDBusConnection::sessionBus(), this))
    , m_theme(new Appearance(Appearance::staticInterfaceName(), "/com/deepin/daemon/Appearance", QDBusConnection::sessionBus(), this))
{
    connect(m_dbus, &Notification::AppAddedSignal, this, &NotificationWorker::onAppAdded);
    connect(m_dbus, &Notification::AppRemovedSignal, this, &NotificationWorker::onAppRemoved);
}

void NotificationWorker::active(bool sync)
{
    if (sync) {
        m_model->clearModel();
        initAllSetting();
    }
}

void NotificationWorker::deactive()
{

}

void NotificationWorker::initAllSetting()
{
    initSystemSetting();
    initAppSetting();
}

void NotificationWorker::initSystemSetting()
{
    SysItemModel *item = new SysItemModel(this);
    item->setTimeStart(m_dbus->GetSystemInfo(SysItemModel::STARTTIME).value().variant().toString());
    item->setTimeEnd(m_dbus->GetSystemInfo(SysItemModel::ENDTIME).value().variant().toString());
    item->setDisturbMode(m_dbus->GetSystemInfo(SysItemModel::DNDMODE).value().variant().toBool());
    item->setLockScreen(m_dbus->GetSystemInfo(SysItemModel::LOCKSCREENOPENDNDMODE).value().variant().toBool());
    item->setTimeSlot(m_dbus->GetSystemInfo(SysItemModel::OPENBYTIMEINTERVAL).value().variant().toBool());
#ifndef USE_TABLET
    item->setShowInDock(m_dbus->GetSystemInfo(SysItemModel::SHOWICON).value().variant().toBool());
#endif
    connect(m_dbus, &Notification::SystemInfoChanged, item, &SysItemModel::onSettingChanged);
    m_model->setSysSetting(item);
}

void NotificationWorker::initAppSetting()
{
    QStringList appList = m_dbus->GetAppList();
    for (int i = 0; i < appList.size(); i++) {
        // laptop-mode-tools 项目crp打包异常，暂时处理为不在通知列表显示
        if (appList[i].contains("laptop-mode-tools", Qt::CaseInsensitive))
            continue;

        onAppAdded(appList[i]);
    }
}

void NotificationWorker::onAppAdded(const QString &id)
{
    AppItemModel *item = new AppItemModel(this);
    item->setActName(id);
    item->setSoftName(m_dbus->GetAppInfo(id, AppItemModel::APPNAME).value().variant().toString());
    item->setIcon(m_dbus->GetAppInfo(id, AppItemModel::APPICON).value().variant().toString());
    item->setAllowNotify(m_dbus->GetAppInfo(id, AppItemModel::ENABELNOTIFICATION).value().variant().toBool());
    item->setShowNotifyPreview(m_dbus->GetAppInfo(id, AppItemModel::ENABELPREVIEW).value().variant().toBool());
    item->setNotifySound(m_dbus->GetAppInfo(id, AppItemModel::ENABELSOUND).value().variant().toBool());
    item->setShowInNotifyCenter(m_dbus->GetAppInfo(id, AppItemModel::SHOWINNOTIFICATIONCENTER).value().variant().toBool());
    item->setLockShowNotify(m_dbus->GetAppInfo(id, AppItemModel::LOCKSCREENSHOWNOTIFICATION).value().variant().toBool());

    connect(m_dbus, &Notification::AppInfoChanged, item, &AppItemModel::onSettingChanged);
    m_model->appAdded(item);
}

void NotificationWorker::onAppRemoved(const QString &id)
{
    m_model->appRemoved(id);
}

void NotificationWorker::setAppSetting(const QString &id, uint item, QVariant var)
{
    m_dbus->SetAppInfo(id, item, QDBusVariant(var));
}

void NotificationWorker::setSystemSetting(uint item, QVariant var)
{
    m_dbus->SetSystemInfo(item, QDBusVariant(var));
}
