// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <qdbusmessage.h>

#include <QObject>
#include <QQmlContext>
#include <QStringList>
#include <QVector>

#include <atomic>

class QQmlComponent;
class QThreadPool;

namespace dccV25 {

enum LoadMode {
    OnlyData,  // 按需：只加载 Data，跳过 Module
    Full       // 全量：完整加载到 PluginEnd
};

class DccObject;
class DccManager;
class DccPluginLoader;

class DccPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit DccPluginManager(DccManager *parent);
    ~DccPluginManager();
    void loadModules(DccObject *root, bool async, const QStringList &dirs, QQmlEngine *engine);
    bool loadFinished() const;
    void beginDelete();

    QQmlEngine *engine();
    DccObject *rootModule();
    bool hidden(const QString &name);

    inline bool isDeleting() const { return m_isDeleting.load(); }

public Q_SLOTS:
    void get(const QString &module, const QVariantMap &param, const QDBusMessage &message);
    void set(const QString &module, const QVariantMap &param, const QDBusMessage &message);
    void switchToFullMode();
    void cancelLoad();

Q_SIGNALS:
    void addObject(DccObject *obj);
    void loadAllFinished();
    void requestAdvancePlugin(DccPluginLoader *loader);

private:
    QThreadPool *threadPool();
    DccPluginLoader *findLoader(const QString &module) const;

private Q_SLOTS:
    void loadPlugin(DccPluginLoader *loader);

private Q_SLOTS:
    void onPluginStatusChanged(DccPluginLoader *loader, uint status);
    void onHideModuleChanged(const QSet<QString> &hideModule);

private:
    DccManager *m_manager;
    QList<DccPluginLoader *> m_plugins; // cache for other plugin
    DccObject *m_rootModule;            // root module from MainWindow
    QThreadPool *m_threadPool;
    std::atomic<bool> m_isDeleting;
    QQmlEngine *m_engine;
    LoadMode m_loadMode = OnlyData;
};

} // namespace dccV25
