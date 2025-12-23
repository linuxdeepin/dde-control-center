// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QQmlContext>
#include <QStringList>
#include <QVector>

class QPluginLoader;
class QQmlComponent;
class QThreadPool;

namespace dccV25 {
class DccObject;
class DccManager;
struct PluginData;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(DccManager *parent);
    ~PluginManager();
    void loadModules(DccObject *root, bool async, const QStringList &dirs);
    bool loadFinished() const;
    void beginDelete();

    inline bool isDeleting() const { return m_isDeleting; }

public Q_SLOTS:
    void cancelLoad();

Q_SIGNALS:
    void addObject(DccObject *obj);
    void loadAllFinished();

    void pluginEndStatusChanged(PluginData *plugin);
    void updatePluginStatus(PluginData *plugin, uint status, const QString &log);

private:
    bool compareVersion(const QString &targetVersion, const QString &baseVersion);
    QThreadPool *threadPool();
    QString pluginQmlPath(PluginData *plugin) const;
    QString pluginMainQmlPath(PluginData *plugin) const;

private Q_SLOTS:
    void loadPlugin(PluginData *plugin);
    void loadMetaData(PluginData *plugin);
    void loadModule(PluginData *plugin);
    void loadMain(PluginData *plugin);
    void createModule(QQmlComponent *component);
    void createMain(QQmlComponent *component);
    void addMainObject(PluginData *plugin);

    void moduleLoading();
    void mainLoading();

    void onHideModuleChanged(const QSet<QString> &hideModule);
    void onVisibleToAppChanged(bool visibleToApp);
    void onUpdatePluginStatus(PluginData *plugin, uint status, const QString &log);

private:
    DccManager *m_manager;
    QList<PluginData *> m_plugins; // cache for other plugin
    DccObject *m_rootModule;       // root module from MainWindow
    QThreadPool *m_threadPool;
    bool m_isDeleting;
};

} // namespace dccV25
