// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appmgr.h"

#include <containment.h>
#include <pluginloader.h>
#include <appletbridge.h>
#include <appletproxy.h>

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <mutex>

Q_LOGGING_CATEGORY(DccNotificationAppMgr, "dde.dcc.notification.appmgr")

namespace {
// Roles exported by dde-shell's org.deepin.ds.dde-apps applet.
class AppGroupManager {
public:
    enum Roles {
        GroupIdRole = Qt::UserRole + 1,
        GroupItemsPerPageRole,
        ExtendRole = 0x1000,
    };
};

class AppItemModel {
public:
    enum Roles {
        DesktopIdRole = AppGroupManager::ExtendRole,
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
        AppTypeRole,
        XLingLongRole,
        IdRole,
        XCreatedByRole,
        ExecsRole,
        CategoriesRole,
        DesktopSourcePathRole,
    };
};
}

AppMgr *AppMgr::instance()
{
    static AppMgr *manager = nullptr;
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [] {
        manager = new AppMgr();
        manager->moveToThread(qApp->thread());
    });
    return manager;
}

AppMgr::AppMgr(QObject *parent)
    : QObject(parent)
{
    initApplet();
}

AppMgr::~AppMgr()
{
    QMutexLocker locker(&m_mutex);
    m_appItems.clear();
}

void AppMgr::initApplet()
{
    DS_NAMESPACE::DAppletBridge bridge("org.deepin.ds.dde-apps");
    auto appletProxy = bridge.applet();
    if (!appletProxy) {
        auto rootApplet = qobject_cast<DS_NAMESPACE::DContainment *>(DS_NAMESPACE::DPluginLoader::instance()->rootApplet());
        if (!rootApplet) {
            qCWarning(DccNotificationAppMgr) << "Failed to get root applet for dde-apps";
            return;
        }

        auto applet = rootApplet->createApplet(DS_NAMESPACE::DAppletData{"org.deepin.ds.dde-apps"});
        if (!applet) {
            qCWarning(DccNotificationAppMgr) << "Failed to create dde-apps applet";
            return;
        }
        applet->load();
        applet->init();
        appletProxy = bridge.applet();
    }

    if (!appletProxy) {
        qCWarning(DccNotificationAppMgr) << "Failed to get applet proxy for dde-apps";
        return;
    }
    m_appModel = appletProxy->property("appModel").value<QAbstractItemModel *>();
    if (!m_appModel) {
        qCWarning(DccNotificationAppMgr) << "Failed to get appModel from dde-apps";
        return;
    }

    connect(m_appModel, &QAbstractItemModel::rowsInserted, this, &AppMgr::onRowsInserted);
    connect(m_appModel, &QAbstractItemModel::rowsAboutToBeRemoved, this,
            &AppMgr::onRowsAboutToBeRemoved, Qt::DirectConnection);
    for (int row = 0; row < m_appModel->rowCount(); ++row)
        createAppItem(row);
}

bool AppMgr::createAppItem(int row)
{
    if (!m_appModel)
        return false;
    const QModelIndex index = m_appModel->index(row, 0);
    const QString id = m_appModel->data(index, AppItemModel::IdRole).toString();
    if (id.isEmpty())
        return false;

    if (m_appModel->data(index, AppItemModel::NoDisplayRole).toBool()) {
        return false;
    }

    DCC_NAMESPACE::NotificationSetting::AppItem item;
    item.id = id;
    item.appName = m_appModel->data(index, AppItemModel::NameRole).toString();
    item.appIcon = m_appModel->data(index, AppItemModel::IconNameRole).toString();

    {
        QMutexLocker locker(&m_mutex);
        if (m_appItems.contains(id)) {
            return false;
        }
        m_appItems.insert(id, item);
    }
    Q_EMIT appItemAdd(id);
    return true;
}

void AppMgr::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    for (int row = first; row <= last; ++row)
        createAppItem(row);
}

void AppMgr::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    if (m_appModel.isNull())
        return;

    QStringList removedIds;
    removedIds.reserve(last - first + 1);
    for (int row = last; row >= first; --row) {
        const QString id = m_appModel->data(m_appModel->index(row, 0), AppItemModel::IdRole).toString();
        if (!id.isEmpty())
            removedIds.append(id);
    }

    {
        QMutexLocker locker(&m_mutex);
        for (const QString &id : removedIds)
            m_appItems.remove(id);
    }

    for (const QString &id : removedIds)
        Q_EMIT appItemRemove(id);
}

QList<DCC_NAMESPACE::NotificationSetting::AppItem> AppMgr::allAppInfosShouldBeShown() const
{
    QMutexLocker locker(&m_mutex);
    return m_appItems.values();
}
