// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "utils.h"

#include <QFuture>
#include <QObject>
#include <QVector>

class QPluginLoader;

namespace DCC_NAMESPACE {
class ModuleObject;
class PluginInterface;
class LayoutManager;
} // namespace DCC_NAMESPACE

struct PluginData
{
    QString Follow;
    QString Location;
    DCC_NAMESPACE::ModuleObject *Module;
    DCC_NAMESPACE::PluginInterface *Plugin;
};

Q_DECLARE_METATYPE(PluginData)

namespace DCC_NAMESPACE {
class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);
    void loadModules(ModuleObject *root,
                     bool async = true,
                     const QStringList &dirs = { PLUGIN_DIRECTORY });
    bool loadFinished() const;

public Q_SLOTS:
    void cancelLoad();

Q_SIGNALS:
    void loadedModule(const PluginData &data);
    void loadAllFinished();

private:
    ModuleObject *findModule(ModuleObject *module, const QString &name);
    void initModules(const PluginData &data);
    void insertChild(bool force);

    QList<PluginData> m_datas;  // cache for other plugin
    ModuleObject *m_rootModule; // root module from MainWindow
    QFuture<PluginData> m_future;
    QVector<QString> m_pluginsStatus;
};

} // namespace DCC_NAMESPACE
