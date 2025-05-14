// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notificationsetting.h"
#include "notificationmodel.h"
#include "appmgr.h"

#include <QVariant>
#include <QAbstractListModel>
#include <QLoggingCategory>

#include <DConfig>

namespace DCC_NAMESPACE {

static const QString InvalidApp {"DS-Invalid-Apps"};
namespace {
enum Roles {
    DesktopIdRole = 0x1000,
    NameRole,
    IconNameRole,
    StartUpWMClassRole,
    NoDisplayRole,
    ActionsRole,
    DDECategoryRole,
    InstalledTimeRole,
    LastLaunchedTimeRole,
    LaunchedTimesRole,
    DockedRole,
    OnDesktopRole,
    AutoStartRole,
    GroupRole,
};
}

NotificationSetting::NotificationSetting(QObject *parent)
    : QObject(parent)
    , m_impl(Dtk::Core::DConfig::create("org.deepin.dde.shell", "org.deepin.dde.shell.notification", QString(), this))
{
    invalidAppItemCached();
    connect(m_impl, &Dtk::Core::DConfig::valueChanged, this, [this] (const QString &key) {
        if (key == "appsInfo") {
            invalidAppItemCached();
        } else {
            static const QStringList
                    keys{"dndMode", "openByTimeInterval", "lockScreenOpenDndMode", "startTime", "endTime", "notificationClosed", "maxCount", "bubbleCount"};
            if (keys.contains(key)) {
                m_systemInfo = {};
                Q_EMIT systemValueChanged(key);
            }
        }
    });
    connect(AppMgr::instance(), &AppMgr::appItemAdd, this, [=] () {onAppsChanged();});
    connect(AppMgr::instance(), &AppMgr::appItemRemove, this, [=] () {onAppsChanged();});
}

void NotificationSetting::setAppValue(const QString &id, AppConfigItem item, const QVariant &value)
{
    auto info = appInfo(id);
    switch (item) {
    case EnableNotification: {
        info["enabled"] = value.toBool();
        break;
    }
    case EnablePreview: {
        info["enablePreview"] = value.toBool();
        break;
    }
    case EnableSound: {
        info["enableSound"] = value.toBool();
        break;
    }
    case ShowInCenter: {
        info["showInCenter"] = value.toBool();
        break;
    }
    case ShowOnLockScreen: {
        info["showOnLockScreen"] = value.toBool();
        break;
    }
    case ShowOnDesktop: {
        info["showOnDesktop"] = value.toBool();
        break;
    }
    default:
        break;
    }

    {
        QMutexLocker locker(&m_appsInfoMutex);
        m_appsInfo[id] = info;
        m_impl->setValue("appsInfo", m_appsInfo);
    }

    Q_EMIT appValueChanged(id, item, value);
}

QVariant NotificationSetting::appValue(const QString &id, AppConfigItem item)
{
    if (item == AppId) {
        return id;
    }

    const auto app = appItem(id);
    switch (item) {
    case AppId: {
        return id;
    }
    case AppName: {
        return app.appName;
    }
    case AppIcon: {
        return app.appIcon;
    default:
        break;
    }
    }

    const auto info = appInfo(id);
    switch (item) {
    case EnableNotification: {
        return info.value("enabled", true);
    }
    case EnablePreview: {
        return info.value("enablePreview", true);
    }
    case EnableSound: {
        return info.value("enableSound", true);
    }
    case ShowInCenter: {
        return info.value("showInCenter", true);
    }
    case ShowOnLockScreen: {
        return info.value("showOnLockScreen", true);
    }
    case ShowOnDesktop: {
        return info.value("showOnDesktop", true);
    }
    default:
        break;
    }

    return QVariant();
}

void NotificationSetting::setSystemValue(NotificationSetting::SystemConfigItem item, const QVariant &value)
{
    switch (item) {
    case DNDMode:
        m_impl->setValue("dndMode", value);
        break;
    case OpenByTimeInterval:
        m_impl->setValue("openByTimeInterval", value);
        break;
    case LockScreenOpenDNDMode:
        m_impl->setValue("lockScreenOpenDndMode", value);
        break;
    case StartTime:
        m_impl->setValue("startTime", value);
        break;
    case EndTime:
        m_impl->setValue("endTime", value);
        break;
    case CloseNotification:
        m_impl->setValue("notificationClosed", value);
        break;
    case MaxCount:
        m_impl->setValue("maxCount", value);
        break;
    case BubbleCount:
        m_impl->setValue("bubbleCount", value);
        break;
    default:
        return;
    }
    m_systemInfo = {};
    Q_EMIT systemValueChanged(item, value);
}

QVariant NotificationSetting::systemValue(NotificationSetting::SystemConfigItem item)
{
    switch (item) {
    case DNDMode:
        return systemValue("dndMode", true);
    case LockScreenOpenDNDMode:
        return systemValue("lockScreenOpenDndMode", false);
    case OpenByTimeInterval:
        return systemValue("openByTimeInterval", true);
    case StartTime:
        return systemValue("startTime", "07:00");
    case EndTime:
        return systemValue("endTime", "22:00");
    case CloseNotification:
        return systemValue("notificationClosed", false);
    case MaxCount:
        return systemValue("maxCount", 2000);
    case BubbleCount:
        return systemValue("bubbleCount", 3);
    }

    return {};
}

QStringList NotificationSetting::apps() const
{
    QStringList ret;
    for (const auto &item : appItems()) {
        ret << item.id;
    }
    return ret;
}

NotificationSetting::AppItem NotificationSetting::appItem(const QString &id) const
{
    const auto infos = appItems();
    auto iter = std::find_if(infos.begin(), infos.end(), [id] (const AppItem &item) {
        return id == item.id;
    });
    if (iter != infos.end()) {
        return *iter;
    }

    return {};
}

QList<NotificationSetting::AppItem> NotificationSetting::appItems() const
{
    QMutexLocker locker(&(const_cast<NotificationSetting *>(this)->m_appItemsMutex));
    if (!m_appItems.isEmpty())
        return m_appItems;

    QList<NotificationSetting::AppItem> apps = appItemsImpl();
    const_cast<NotificationSetting *>(this)->m_appItems = apps;
    return m_appItems;
}

QList<NotificationSetting::AppItem> NotificationSetting::appItemsImpl() const
{
    QList<NotificationSetting::AppItem> appSettings;
    QList<AppMgr::AppItem *> apps = AppMgr::instance()->allAppInfosShouldBeShown();
    for (int i = 0; i < apps.count(); i++) {
        const auto desktopId = apps[i]->appId;
        const auto icon = apps[i]->iconName;
        const auto name = apps[i]->displayName;

        NotificationSetting::AppItem item;
        item.id = desktopId;
        item.appIcon = icon;
        item.appName = name;
        appSettings << item;
    }
    return appSettings;
}

QVariantMap NotificationSetting::appInfo(const QString &id) const
{
    QMutexLocker locker(&(const_cast<NotificationSetting *>(this)->m_appsInfoMutex));
    if (m_appsInfo.contains(InvalidApp)) {
        const_cast<NotificationSetting *>(this)->m_appsInfo = m_impl->value("appsInfo").toMap();
    }
    if (auto iter = m_appsInfo.find(id); iter != m_appsInfo.end()) {
        return iter.value().toMap();
    }
    return {};
}

void NotificationSetting::onAppsChanged()
{
    const auto old = appItems();
    const auto current = appItemsImpl();
    {
        QMutexLocker locker(&m_appItemsMutex);
        m_appItems = current;
    }

    QList<NotificationSetting::AppItem> added;
    for (const auto &item : current) {
        const auto id = item.id;
        auto iter = std::find_if(old.begin(), old.end(), [id] (const NotificationSetting::AppItem &app) {
            return id == app.id;
        });
        if (iter == old.end()) {
            added << item;
        }
    }
    for (const auto &item : added) {
        Q_EMIT appAdded(item.id);
    }

    QList<NotificationSetting::AppItem> removed;
    for (const auto &item : old) {
        const auto id = item.id;
        auto iter = std::find_if(current.begin(), current.end(), [id] (const NotificationSetting::AppItem &app) {
            return id == app.id;
        });
        if (iter == current.end()) {
            removed << item;
        }
    }
    for (const auto &item : removed) {
        Q_EMIT appRemoved(item.id);
    }

}

void NotificationSetting::invalidAppItemCached()
{
    QMutexLocker locker(&m_appsInfoMutex);
    m_appsInfo.clear();
    m_appsInfo[InvalidApp] = QVariant();
}

QVariant NotificationSetting::systemValue(const QString &key, const QVariant &fallback)
{
    if (!m_systemInfo.contains(key))
        m_systemInfo[key] = m_impl->value(key, fallback);
    return m_systemInfo[key];
}

} // notification
