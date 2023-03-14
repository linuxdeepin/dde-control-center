//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
#include "interface/hlistmodule.h"

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
class VListModule;
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

class NotificationModule  : public DCC_NAMESPACE::HListModule {
    Q_OBJECT
public:
    explicit NotificationModule(QObject *parent = nullptr);
    virtual void active() override;
private Q_SLOTS:
    void initUi();
    NotificationWorker *work() { return m_worker; }
    NotificationModel *model() { return m_model; }
    void onAppListAdded(AppItemModel *item);
    void onAppListRemoved(AppItemModel *item);
private:
    NotificationModel *m_model;
    NotificationWorker *m_worker;
    VListModule *m_appNotify;
    QStringList m_appNameList;
};
}
