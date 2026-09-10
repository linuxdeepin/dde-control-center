// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccdsappletmanager.h"

#include <appletbridge.h>
#include <containment.h>
#include <pluginloader.h>

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QLoggingCategory>


Q_LOGGING_CATEGORY(DccDSAppletManager, "dde.dcc.dsappletmanager")

namespace {
constexpr const char *kAppletUri = "org.deepin.ds.dde-apps";
}

static DS_NAMESPACE::DAppletProxy *initApplet(const QString &uri)
{
    DS_NAMESPACE::DAppletBridge bridge(uri);
    auto *appletProxy = bridge.applet();
    if (!appletProxy) {
        auto *rootApplet = qobject_cast<DS_NAMESPACE::DContainment *>(DS_NAMESPACE::DPluginLoader::instance()->rootApplet());
        if (!rootApplet) {
            qCWarning(DccDSAppletManager) << "Failed to get root applet";
            return nullptr;
        }
        auto applet = rootApplet->createApplet(DS_NAMESPACE::DAppletData{ uri });
        if (!applet) {
            qCWarning(DccDSAppletManager) << "Failed to create applet";
            return nullptr;
        }
        applet->load();
        applet->init();
        appletProxy = bridge.applet();
    }
    if (!appletProxy) {
        qCWarning(DccDSAppletManager) << "Failed to get applet proxy for" << uri;
    }
    return appletProxy;
}

DSAppletManager::DSAppletManager(QObject *parent)
    : QObject(parent)
{
    DS_NAMESPACE::DAppletProxy *appletProxy = initApplet(kAppletUri);
    if (appletProxy) {
        m_appModel = appletProxy->property("appModel").value<QAbstractItemModel *>();
        if (!m_appModel) {
            qCWarning(DccDSAppletManager) << "Failed to get appModel from" << kAppletUri;
        }
    }
    if (!DS_NAMESPACE::DPluginLoader::instance()->parent()) {
        qCInfo(DccDSAppletManager) << "DPluginLoader has no parent, fixing";
        DS_NAMESPACE::DPluginLoader::instance()->moveToThread(qApp->thread());
        DS_NAMESPACE::DPluginLoader::instance()->setParent(qApp);
    }
    auto rootApplet = DS_NAMESPACE::DPluginLoader::instance()->rootApplet();
    if (rootApplet && !rootApplet->parent()) {
        qCInfo(DccDSAppletManager) << "rootApplet has no parent, fixing";
        rootApplet->moveToThread(qApp->thread());
        rootApplet->setParent(qApp);
    }
    moveToThread(qApp->thread());
}

DSAppletManager *DSAppletManager::instance()
{
    // 线程安全懒创建（C++11 magic static）；构造时即完成 applet 初始化，init() 可在任意线程调用。
    static auto *s_instance = new DSAppletManager();
    return s_instance;
}

QAbstractItemModel *DSAppletManager::appModel() const
{
    return m_appModel.data();
}
