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
#pragma once

#include "interface/namespace.h"
#include "notificationmodel.h"
#include <com_deepin_dde_notification.h>
#include <com_deepin_daemon_appearance.h>

#include <QObject>

using Notification = com::deepin::dde::Notification;
using Appearance = com::deepin::daemon::Appearance;

namespace dcc {
namespace notification {

class NotificationModel;
class NotificationWorker : public QObject
{
    Q_OBJECT
public:
    explicit NotificationWorker(NotificationModel *model, QObject *parent = nullptr);
    void active(bool sync);
    void deactive();

public Q_SLOTS:
    void setBusSysnotify(const QJsonObject &jObj);
    void setBusAppnotify(const QJsonObject &jObj);
    void setBusAppnotify(const QString &appName, const QJsonObject &jObj);
    void getDbusAllSetting(const QString &jObj = "");
    void getDbusAppsetting(const QString &jObj);
    void getDbusSyssetting(const QString &jObj = "");
    void setIconTheme(const QString &theme);

private:
    NotificationModel *m_model;
    Notification *m_dbus;
    Appearance *m_theme;
};

}// namespace msgnotify
}// namespace dcc
