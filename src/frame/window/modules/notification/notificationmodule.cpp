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
#include "modules/notification/notificationmodel.h"
#include "modules/notification/notificationworker.h"
#include "modules/notification/model/appitemmodel.h"
#include "systemnotifywidget.h"
#include "appnotifywidget.h"
#include "window/mainwindow.h"

#include <DListView>

using namespace dcc;
using namespace dcc::notification;
using namespace DCC_NAMESPACE::notification;

NotificationModule::NotificationModule(dccV20::FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_widget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

NotificationModule::~NotificationModule()
{
    m_worker->deleteLater();
    m_model->deleteLater();
}

// 控制中心启动时会被调用
void NotificationModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync)
    Q_UNUSED(pushtype);
    if (m_model) {
        delete m_model;
    }
    m_model = new NotificationModel(this);
    m_worker = new NotificationWorker(m_model, this);
    m_worker->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
    m_worker->active(true); //refresh data
}

// 仅在进入模块是会被调用
void NotificationModule::initialize()
{

}

const QString NotificationModule::name() const
{
    return QStringLiteral("notification");
}

const QString NotificationModule::displayName() const
{
    return tr("Notification");
}

void NotificationModule::active()
{
    m_worker->active(true);

    m_widget = new NotificationWidget(m_model);
    m_widget->setVisible(false);
    connect(m_widget, &NotificationWidget::requestShowSystem, this, &NotificationModule::showSystemNotify);
    connect(m_widget, &NotificationWidget::requestShowApp, this, &NotificationModule::showAppNotify);
    connect(m_widget, &NotificationWidget::popWidget, this, [ = ] {
        if (m_pMainWindow->getcontentStack().size() >= 2)
            m_frameProxy->popWidget(this);
    });
    m_frameProxy->pushWidget(this, m_widget);
    m_widget->setVisible(true);
    m_widget->showDefaultWidget();
}

int NotificationModule::load(const QString &path)
{
    if (!m_widget) {
        active();
    }

    int index = availPage().indexOf(path);
    if (index == 0) {
        m_widget->getSysListview()->clicked(m_widget->getSysListview()->model()->index(0, 0));
    } else if (index > 0) {
        m_widget->getAppListview()->clicked(m_widget->getAppListview()->model()->index(index - 1, 0));
    }
    return 0;
}

QStringList NotificationModule::availPage() const
{
    QStringList list;
    list << "System Notification";
    for(int i = 0; i < m_model->getAppSize(); i++) {
        list << m_model->getAppModel(i)->getAppName();
    }
    return list;
}

void NotificationModule::showSystemNotify()
{
    SystemNotifyWidget *widget = new SystemNotifyWidget(m_model->getSystemModel(), m_widget);
    widget->setVisible(false);
    connect(widget, &SystemNotifyWidget::requestSetSysSetting, m_worker, &NotificationWorker::setSystemSetting);
    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}

void NotificationModule::showAppNotify(int index)
{
    AppNotifyWidget *widget = new AppNotifyWidget(m_model->getAppModel(index), m_widget);
    widget->setVisible(false);
    connect(widget, &AppNotifyWidget::requestSetAppSetting, m_worker, &NotificationWorker::setAppSetting);
    m_frameProxy->pushWidget(this, widget);
    widget->setVisible(true);
}
