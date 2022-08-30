// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    Notification *getDbusObject() { return m_dbus; }

public Q_SLOTS:
    void initAllSetting();
    void initSystemSetting();
    void initAppSetting();
    void onAppAdded(const QString &id);
    void onAppRemoved(const QString &id);
    void setAppSetting(const QString &id, uint item, QVariant var);
    void setSystemSetting(uint item, QVariant var);

private:
    NotificationModel *m_model;
    Notification *m_dbus;
    Appearance *m_theme;
};

}// namespace msgnotify
}// namespace dcc
