//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "notificationmodel.h"
#include "model/sysitemmodel.h"
#include "model/appitemmodel.h"
#include "dccfactory.h"
#include "appssourcemodel.h"
#include "appslistmodel.h"
#include "operation/notificationsetting.h"
#include "appmgr.h"

using namespace DCC_NAMESPACE;

#define SYSTEMNOTIFY_NAME "SystemNotify"

NotificationModel::NotificationModel(QObject *parent)
    : QObject(parent)
    , m_setting(new NotificationSetting(this))
    , m_sysItemModel(new SysItemModel(m_setting, this))
    , m_appsSourceModel(new AppsSourceModel(this))
    , m_appsListModel(new AppsListModel(this))
{
    m_appsListModel->setSourceModel(m_appsSourceModel);
    m_appsListModel->sort(0);

    auto addAppItem = [=] (const QString &appId) {
        AppItemModel *item = new AppItemModel(m_setting, this);
        item->setActName(m_setting->appValue(appId, NotificationSetting::AppId).toString());
        item->setSoftName(m_setting->appValue(appId, NotificationSetting::AppName).toString());
        item->setIcon(m_setting->appValue(appId, NotificationSetting::AppIcon).toString());
        item->setAllowNotify(m_setting->appValue(appId, NotificationSetting::EnableNotification).toBool());
        item->setShowNotifyPreview(m_setting->appValue(appId, NotificationSetting::EnablePreview).toBool());
        item->setNotifySound(m_setting->appValue(appId, NotificationSetting::EnableSound).toBool());
        item->setShowInNotifyCenter(m_setting->appValue(appId, NotificationSetting::ShowInCenter).toBool());
        item->setLockShowNotify(m_setting->appValue(appId, NotificationSetting::ShowOnLockScreen).toBool());
        item->setShowDesktop(m_setting->appValue(appId, NotificationSetting::ShowOnDesktop).toBool());
        m_appsSourceModel->appAdded(item);
    };

    for (const auto &app : m_setting->apps()) {
        addAppItem(app);
    }
    connect(m_setting, &NotificationSetting::appAdded, this, [=] (const QString &appId) {
        addAppItem(appId);
    });
    connect(m_setting, &NotificationSetting::appRemoved, this, &NotificationModel::appRemoved);
}

NotificationModel::~NotificationModel()
{

}

void NotificationModel::setSysSetting(SysItemModel *item)
{
    m_sysItemModel = item;
}

void NotificationModel::clearModel()
{
    m_sysItemModel->deleteLater();
    m_sysItemModel = nullptr;
    qDeleteAll(m_appItemModels);
    m_appItemModels.clear();
}

void NotificationModel::appAdded(AppItemModel *item)
{
    m_appsSourceModel->appAdded(item);
}

void NotificationModel::appRemoved(const QString &appName)
{
    m_appsSourceModel->appRemoved(appName);
}

DCC_FACTORY_CLASS(NotificationModel)
#include "notificationmodel.moc"

