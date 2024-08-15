// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "notificationmodule.h"

#include "appnotifywidget.h"
#include "interface/pagemodule.h"
#include "itemmodule.h"
#include "src/plugin-notification/operation/model/appitemmodel.h"
#include "src/plugin-notification/operation/notificationmodel.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "systemnotifywidget.h"
#include "vlistmodule.h"

#include <DIconTheme>

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QSvgRenderer>

Q_LOGGING_CATEGORY(DccNotifyModule, "dcc-notify-module");

Q_DECLARE_METATYPE(QMargins)
using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE

QString NotificationPlugin::name() const
{
    return QStringLiteral("notification");
}

ModuleObject *NotificationPlugin::module()
{
    return new NotificationModule;
}

QString NotificationPlugin::location() const
{
    return "7";
}

NotificationModule::NotificationModule(QObject *parent)
    : HListModule(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_appNotify(new VListModule("AppNotify", tr("AppNotify"), this))
    , m_appNameList{}
{
    setName("notification");
    setDisplayName(tr("Notification"));
    setIcon(DIconTheme::findQIcon("dcc_nav_notification"));
    if (m_model) {
        delete m_model;
    }
    m_model = new NotificationModel(this);
    m_worker = new NotificationWorker(m_model, this);
    connect(m_model, &NotificationModel::appListAdded, this, &NotificationModule::onAppListAdded);
    // clang-format off
    connect(m_model, &NotificationModel::appListRemoved,
            this, &NotificationModule::onAppListRemoved);
    // clang-format on
    initUi();
}

void NotificationModule::active()
{
    if (m_model->getAppSize() == 0)
        m_worker->active(true);
}

void NotificationModule::initUi()
{
    ModuleObject *systemNotify = new PageModule("SystemNotify", tr("SystemNotify"), this);
    systemNotify->appendChild(new ItemModule(
            "SystemNotify",
            "SystemNotify",
            [this](ModuleObject *module) {
                Q_UNUSED(module)
                auto sysNotifyWidget = new SystemNotifyWidget(m_model->getSystemModel());
                connect(sysNotifyWidget,
                        &SystemNotifyWidget::requestSetSysSetting,
                        m_worker,
                        &NotificationWorker::setSystemSetting);
                return sysNotifyWidget;
            },
            false));
    appendChild(systemNotify);

    appendChild(m_appNotify);
}

void NotificationModule::onAppListAdded(AppItemModel *item)
{
    QString softName = item->getAppName();
    qCInfo(DccNotifyModule) << "App" << softName << "added";
    QIcon icon = DIconTheme::findQIcon(item->getIcon());
    m_appNameList.append(softName);
    PageModule *newpage = new PageModule(softName, softName, icon, nullptr);
    newpage->appendChild(new ItemModule(
                                 softName,
                                 softName,
                                 [item, this](ModuleObject *module) {
                                     Q_UNUSED(module)
                                     auto notifyWidget = new AppNotifyWidget(item);
                                     notifyWidget->setSizePolicy(QSizePolicy::Expanding,
                                                                 QSizePolicy::Expanding);
                                     connect(notifyWidget,
                                             &AppNotifyWidget::requestSetAppSetting,
                                             m_worker,
                                             &NotificationWorker::setAppSetting);
                                     return notifyWidget;
                                 },
                                 false),
                         false);
    m_appNotify->appendChild(newpage);
}

void NotificationModule::onAppListRemoved(AppItemModel *item)
{

    int index = m_appNameList.indexOf(item->getAppName());
    qCInfo(DccNotifyModule) << "App" << item->getAppName() << "removed";
    if (index >= 0) {
        m_appNameList.removeAt(index);
        m_appNotify->removeChild(index);
    }
}
