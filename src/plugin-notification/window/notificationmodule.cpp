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
#include "notificationmodule.h"
#include "notificationwidget.h"
#include "src/plugin-notification/operation/notificationmodel.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "src/plugin-notification/operation/model/appitemmodel.h"
#include "systemnotifywidget.h"
#include "appnotifywidget.h"

#include <DListView>

#include <QApplication>

DCC_USE_NAMESPACE
QString NotificationPlugin::name() const
{
    return QStringLiteral("notification");
}

ModuleObject *NotificationPlugin::module()
{
    //一级菜单--通知
    ModuleObject *moduleInterface = new ModuleObject(this);
    ModuleData *dataRoot = new ModuleData(this);
    dataRoot->Name = tr("Notification");
    dataRoot->DisplayName = tr("Notification");
    dataRoot->Description = tr("Notification");
    dataRoot->Icon = QIcon::fromTheme("dcc_nav_notification");
    moduleInterface->setModuleData(dataRoot);
    moduleInterface->setChildType(ModuleObject::ChildType::Page);

    NotificationModule *displayModule = new NotificationModule(moduleInterface);
    moduleInterface->appendChild(displayModule);
    return moduleInterface;
}


NotificationModule::NotificationModule(QObject *parent)
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_notificationWidget(nullptr)
    , m_contentLayout(nullptr)
    , m_appNotifyWidget(nullptr)
    , m_systemNotifyWidget(nullptr)
{
    if (m_model) {
        delete m_model;
    }
    m_model = new NotificationModel(this);
    m_worker = new NotificationWorker(m_model, this);
}

NotificationModule::~NotificationModule()
{
    m_worker->deleteLater();
    m_model->deleteLater();
}

void NotificationModule::active()
{
    m_worker->active(true);
}

QWidget *NotificationModule::page()
{
    QWidget *w = new QWidget();
    m_notificationWidget = new NotificationWidget(m_model);
    m_contentLayout = new QHBoxLayout;
    m_contentLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->insertWidget(0, m_notificationWidget);
    m_notificationWidget->setVisible(false);
    connect(m_notificationWidget, &NotificationWidget::requestShowSystem, this, &NotificationModule::showSystemNotify);
    connect(m_notificationWidget, &NotificationWidget::requestShowApp, this, &NotificationModule::showAppNotify);
    m_notificationWidget->setVisible(true);
    m_notificationWidget->showDefaultWidget();

    w->setLayout(m_contentLayout);
    return w;
}

void NotificationModule::showSystemNotify()
{
    m_contentLayout->removeWidget(m_appNotifyWidget);
    if(m_appNotifyWidget) {
        delete m_appNotifyWidget;
        m_appNotifyWidget = nullptr;
    }

    m_contentLayout->removeWidget(m_systemNotifyWidget);
    if(m_systemNotifyWidget) {
        delete m_systemNotifyWidget;
        m_systemNotifyWidget = nullptr;
    }

    m_systemNotifyWidget = new SystemNotifyWidget(m_model->getSystemModel());
    m_contentLayout->insertWidget(1, m_systemNotifyWidget);
    m_systemNotifyWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(m_systemNotifyWidget, &SystemNotifyWidget::requestSetSysSetting, m_worker, &NotificationWorker::setSystemSetting);
}

void NotificationModule::showAppNotify(int index)
{
    m_contentLayout->removeWidget(m_appNotifyWidget);
    if(m_appNotifyWidget) {
        delete m_appNotifyWidget;
        m_appNotifyWidget = nullptr;
    }

    m_contentLayout->removeWidget(m_systemNotifyWidget);
    if(m_systemNotifyWidget) {
        delete m_systemNotifyWidget;
        m_systemNotifyWidget = nullptr;
    }

    m_appNotifyWidget = new AppNotifyWidget(m_model->getAppModel(index));
    m_contentLayout->insertWidget(1, m_appNotifyWidget);
    m_appNotifyWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(m_appNotifyWidget, &AppNotifyWidget::requestSetAppSetting, m_worker, &NotificationWorker::setAppSetting);
}

void NotificationModule::deactive()
{
    m_notificationWidget = nullptr;
    m_appNotifyWidget = nullptr;
    m_systemNotifyWidget = nullptr;
    m_contentLayout = nullptr;
}
