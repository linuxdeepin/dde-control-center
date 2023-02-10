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
#include "notificationdbusproxy.h"
#include "notificationmodel.h"

#include <QObject>

namespace DCC_NAMESPACE {

class NotificationModel;

class NotificationWorker : public QObject
{
    Q_OBJECT
public:
    explicit NotificationWorker(NotificationModel *model, QObject *parent = nullptr);
    void active(bool sync);
    void deactive();

    NotificationDBusProxy *getDbusObject() { return m_dbus; }

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
    NotificationDBusProxy *m_dbus;
};
} // namespace DCC_NAMESPACE
