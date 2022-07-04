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
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include <QObject>
#include <QPointer>

class QHBoxLayout;
class QStandardItemModel;

namespace DCC_NAMESPACE {

class NotificationWorker;
class NotificationModel;
class NotificationWidget;
class AppNotifyWidget;
class SystemNotifyWidget;
class AppItemModel;

class NotificationPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Notification" FILE "NotificationPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

class NotificationModule: public ModuleObject
{
    Q_OBJECT
public:
    explicit NotificationModule(QObject *parent = nullptr);
    virtual ~NotificationModule() override;
    virtual void active() override;
    virtual void deactive() override;

    virtual QWidget *page() override;
    NotificationWorker *work() { return m_worker; }
    NotificationModel *model() { return m_model; }

private Q_SLOTS:
    void showSystemNotify();
    void showAppNotify(int index);
    void onAppListAdded(AppItemModel *item);
    void onAppListRemoved(AppItemModel *item);

private:
    NotificationModel *m_model;
    NotificationWorker *m_worker;
    QPointer<NotificationWidget> m_notificationWidget;
    QHBoxLayout *m_contentLayout;
    AppNotifyWidget *m_appNotifyWidget;
    SystemNotifyWidget *m_systemNotifyWidget;
    QStandardItemModel *m_softwaremodel;
};
}
