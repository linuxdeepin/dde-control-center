// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "dccbenchmark.h"

#include <QObject>
#include <QQmlContext>
#include <QStringList>
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

private:
    QThreadPool *threadPool();

private Q_SLOTS:
    void loadPlugin(DccPluginLoader *loader);

private Q_SLOTS:
    void onPluginStatusChanged(DccPluginLoader *loader, uint status);
    void onHideModuleChanged(const QSet<QString> &hideModule);

private:
    void checkNavigationFinished();
    void checkLoadFinished();

    DccManager *m_manager;
    QList<DccPluginLoader *> m_plugins; // cache for other plugin
    DccObject *m_rootModule;            // root module from MainWindow
    QThreadPool *m_threadPool;
    std::atomic<bool> m_isDeleting;
    QQmlEngine *m_engine;
    QStringList m_pluginsToLoad;
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
