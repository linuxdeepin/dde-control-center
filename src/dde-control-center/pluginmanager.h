// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

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
    void loadModules(DccObject *root, bool async, const QStringList &dirs, QQmlEngine *engine);
    bool loadFinished() const;
    void beginDelete();

    QQmlEngine *engine();
    DccObject *rootModule();
    bool hidden(const QString &name);

    inline bool isDeleting() const { return m_isDeleting.load(); }

public Q_SLOTS:
    void cancelLoad();

Q_SIGNALS:
    void addObject(DccObject *obj);
    void loadAllFinished();

private:
    QThreadPool *threadPool();

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
};

} // namespace dccV25
