//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "pluginmanagerv20.h"
#include "moduleinterface.h"
#include "frameproxyinterface.h"
#include "adapterv20tov23module.h"

#include <QDir>
#include <QLibrary>
#include <QElapsedTimer>
#include <QPluginLoader>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcAdapterV20toV23Worker, "dcc-adapterv20tov23-worker")
const QString &PluginDirectory = QStringLiteral(DCC_DefaultModuleDirectory);

using namespace DCC_NAMESPACE;
using namespace dccV20;

PluginManagerV20::PluginManagerV20()
{
}

PluginManagerV20::~PluginManagerV20()
{
}

QStringList PluginManagerV20::pluginPath()
{
    QStringList paths;
    QDir moduleDir(PluginDirectory);
    if (!moduleDir.exists()) {
        qCWarning(DdcAdapterV20toV23Worker) << "plugin directory not exists";
        return paths;
    }
    auto moduleList = moduleDir.entryInfoList();
    for (auto i : moduleList) {
        QString path = i.absoluteFilePath();
        if (!QLibrary::isLibrary(path))
            continue;
        paths.append(path);
    }
    return paths;
}

void PluginManagerV20::loadPlugin(QString path, dccV20::FrameProxyInterface *frameProxy)
{
    Q_UNUSED(frameProxy)
    qCDebug(DdcAdapterV20toV23Worker) << "loading module: " << path;
    QElapsedTimer et;
    et.start();
    QPluginLoader loader(path);

    QObject *instance = loader.instance();
    if (!instance) {
        qDebug() << loader.errorString();
        return;
    }

    auto *module = qobject_cast<ModuleInterface *>(instance);
    if (!module) {
        return;
    }
    qCDebug(DdcAdapterV20toV23Worker) << "load plugin Name: " << module->name() << module->displayName();
    qCDebug(DdcAdapterV20toV23Worker) << "load this plugin using time: " << et.elapsed() << "ms";
    m_modules.append(new AdapterV20toV23Module(module));
}
