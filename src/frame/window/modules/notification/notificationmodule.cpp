// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "notificationmodule.h"
#include "notificationwidget.h"
#include "modules/notification/notificationmodel.h"
#include "modules/notification/notificationworker.h"
#include "modules/notification/model/appitemmodel.h"
#include "systemnotifywidget.h"
#include "appnotifywidget.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

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
    GSettingWatcher::instance()->insertState("systemNotification");
    GSettingWatcher::instance()->insertState("appNotifications");
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

    addChildPageTrans();
    initSearchData();
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
    m_worker->active(false);

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

void NotificationModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //notification
        m_frameProxy->addChildPageTrans("System Notifications", tr("System Notifications"));
        m_frameProxy->addChildPageTrans("App Notifications", tr("App Notifications"));
    }
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

void NotificationModule::initSearchData()
{
    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [ = ](const QString &gsettings) {
        if (gsettings == "") {
            return false;
        }

        bool ret = false;
        ret = GSettingWatcher::instance()->get(gsettings).toBool();
        gsettingsMap.insert(gsettings, ret);

        return ret;
    };

    const QString& module = displayName();

    auto func_process_all = [ = ]() {
        m_frameProxy->setWidgetVisible(module, tr("System Notifications"), func_is_visible("systemNotification"));
        m_frameProxy->setWidgetVisible(module, tr("App Notifications"), func_is_visible("appNotifications"));
        m_frameProxy->setDetailVisible(module, tr("System Notifications"), tr("Do Not Disturb"), func_is_visible("systemNotification"));
        m_frameProxy->setDetailVisible(module, tr("System Notifications"), tr("Show icon on Dock"), func_is_visible("systemNotification"));
     };

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if (gsetting == "" || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
            return;
        }

        if ("systemNotification" == gsetting) {
            m_frameProxy->setDetailVisible(module, tr("System Notifications"), tr("Do Not Disturb"), func_is_visible("systemNotification"));
            m_frameProxy->setDetailVisible(module, tr("System Notifications"), tr("Show icon on Dock"), func_is_visible("systemNotification"));
        } else if ("appNotifications" == gsetting) {
            m_frameProxy->setWidgetVisible(module, tr("App Notifications"), func_is_visible("appNotifications"));
        } else {
            qWarning() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}
