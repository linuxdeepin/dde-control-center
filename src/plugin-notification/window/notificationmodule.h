//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
