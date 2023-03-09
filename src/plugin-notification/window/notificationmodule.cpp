//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "notificationmodule.h"
#include "notificationwidget.h"
#include "interface/pagemodule.h"
#include "src/plugin-notification/operation/notificationmodel.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "src/plugin-notification/operation/model/appitemmodel.h"
#include "systemnotifywidget.h"
#include "appnotifywidget.h"

#include <DListView>

#include <DStandardItem>
#include <QApplication>

Q_DECLARE_METATYPE(QMargins)
using namespace DCC_NAMESPACE;
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
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_notificationWidget(nullptr)
    , m_contentLayout(nullptr)
    , m_appNotifyWidget(nullptr)
    , m_systemNotifyWidget(nullptr)
    , m_softwaremodel(new QStandardItemModel(this))
{
    setName("notification");
    setDisplayName(tr("Notification"));
    setDescription(tr("Notification"));
    setIcon(QIcon::fromTheme("dcc_nav_notification"));
    if (m_model) {
        delete m_model;
    }
    m_model = new NotificationModel(this);
    m_worker = new NotificationWorker(m_model, this);

    connect(m_model, &NotificationModel::appListAdded, this, &NotificationModule::onAppListAdded);
    connect(m_model, &NotificationModel::appListRemoved, this, &NotificationModule::onAppListRemoved);
}

NotificationModule::~NotificationModule()
{
    m_worker->deleteLater();
    m_model->deleteLater();
}

void NotificationModule::active()
{
    if (m_model->getAppSize() == 0)
        m_worker->active(true);
}

QWidget *NotificationModule::page()
{
    QWidget *w = new QWidget();
    m_notificationWidget = new NotificationWidget(m_model, m_softwaremodel);
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

void NotificationModule::onAppListAdded(AppItemModel *item)
{
    QString softName = item->getAppName();
    QIcon icon = NotificationWidget::getAppIcon(item->getIcon(), QSize(32, 32));
    DTK_WIDGET_NAMESPACE::DStandardItem *dItem = new DTK_WIDGET_NAMESPACE::DStandardItem(icon, softName);
    dItem->setData(QVariant::fromValue(QMargins(10, 8, 10, 8)), Dtk::MarginsRole);
    m_softwaremodel->appendRow(dItem);
}

void NotificationModule::onAppListRemoved(AppItemModel *item)
{
    for (int i = m_softwaremodel->rowCount() - 1; i >= 0; --i) {
        if (m_softwaremodel->item(i)->text() == item->getAppName()) {
            m_softwaremodel->removeRow(i);
            break;
        }
    }
}

void NotificationModule::deactive()
{
    m_notificationWidget = nullptr;
    m_appNotifyWidget = nullptr;
    m_systemNotifyWidget = nullptr;
    m_contentLayout = nullptr;
}
