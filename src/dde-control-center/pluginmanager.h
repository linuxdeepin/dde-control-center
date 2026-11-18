// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "dccbenchmark.h"

#include <QObject>
#include <QQmlContext>
#include <QStringList>
#include <QSet>
#include <QVector>

#include <atomic>

class QQmlComponent;
class QThreadPool;

namespace dccV25 {
class DccObject;
class DccManager;
class DccPluginLoader;

class DccPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit DccPluginManager(DccManager *parent);
    ~DccPluginManager();
    void setPlugins(const QStringList &plugins);
    void loadModules(DccObject *root, bool async, const QStringList &dirs, QQmlEngine *engine);
    bool loadFinished() const;
    void beginDelete();
    void startDataPhase();

    // 按插件 ID（目录名/loader 名）精确查找；重名时返回 nullptr 并记录告警
    DccPluginLoader *findLoader(const QString &module) const;
    // 插件发现阶段是否已结束：未结束前不能把"找不到"判定为 NotFound
    bool discoveryFinished() const;

    QQmlEngine *engine();
    DccObject *rootModule();
    bool hidden(const QString &name);
    void setShowOnNavigationReady(bool enabled);

    inline bool isDeleting() const { return m_isDeleting.load(); }
    inline bool showOnNavigationReady() const { return m_showOnNavigationReady; }

public Q_SLOTS:
    void cancelLoad();

Q_SIGNALS:
    void addObject(DccObject *obj);
    void moduleLoaded(const QString &name);
    void navigationReady();
    void loadAllFinished();
    // 插件发现阶段结束：请求层据此收束找不到 loader 的排队请求
    void discoveryFinishedChanged();
    // 某个 loader 的数据后端进入终态（Ready/Failed/NotSupported），每个 loader 恰好一次
    void backendStateChanged(DccPluginLoader *loader);
    void modulePropertiesChanged(const QString &module, const QVariantMap &properties);

private:
    QThreadPool *threadPool();

private Q_SLOTS:
    void loadPlugin(DccPluginLoader *loader);
    void onPluginStatusChanged(DccPluginLoader *loader, uint status);
    // v2：worker 完成动态库加载后回到主线程，先执行 initialized() 再调度 create
    void onFactoryReady(DccPluginLoader *loader);
    void onFactoryPropertiesChanged(const QVariantMap &properties);
    void onHideModuleChanged(const QSet<QString> &hideModule);

private:
    void connectFactorySignals(DccPluginLoader *loader);
    void checkNavigationFinished();
    void checkLoadFinished();

    DccManager *m_manager;
    QList<DccPluginLoader *> m_plugins; // cache for other plugin
    DccObject *m_rootModule;            // root module from MainWindow
    QThreadPool *m_threadPool;
    std::atomic<bool> m_isDeleting;
    QQmlEngine *m_engine;
    QStringList m_pluginsToLoad;
    // 已登记 propertiesChanged 的 v2 工厂，避免重复连接
    QSet<const void *> m_connectedFactories;
    DccLoadTimer m_loadTimer;
    bool m_navigationFinished = false;
    // 数据阶段是否已启动。首次由 DccManager::handleNavigationReady() 触发；
    // 之后用于识别"模块已就绪但数据阶段尚未为它启动"的插件（加载时被判为隐藏、
    // 随后才变为可见），为其补跑数据阶段。
    bool m_dataPhaseStarted = false;
    bool m_allLoadFinished = false;
    bool m_showOnNavigationReady;
};

} // namespace dccV25
