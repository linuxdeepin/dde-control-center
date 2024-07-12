// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QFuture>
#include <QObject>
#include <QQmlContext>
#include <QStringList>
#include <QVector>

class QPluginLoader;
class QQmlComponent;

namespace dccV25 {
class DccObject;
class DccManager;
class PluginData;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(DccManager *parent);
    void loadModules(DccObject *root, bool async, const QStringList &dirs);
    bool loadFinished() const;

public Q_SLOTS:
    void cancelLoad();

Q_SIGNALS:
    void addObject(DccObject *obj);
    void loadedModule(const PluginData &data);
    void loadAllFinished();
    void loadOsFinished(PluginData *plugin);

private:
    bool compareVersion(const QString &targetVersion, const QString &baseVersion);

private Q_SLOTS:
    void loadModule(PluginData *plugin);
    void loadMain(PluginData *plugin);
    void createModule(QQmlComponent *component);
    void createMain(QQmlComponent *component);
    void addMainObject(PluginData *plugin);

    void moduleLoading();
    void mainLoading();

private:
    DccManager *m_manager;
    QList<PluginData *> m_plugins; // cache for other plugin
    DccObject *m_rootModule;       // root module from MainWindow
    QFuture<void> m_future;
    QVector<QString> m_pluginsStatus;
};

} // namespace dccV25
