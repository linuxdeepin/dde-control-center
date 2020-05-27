/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pluginscontroller.h"
#include "interfaces/plugininterface.h"

#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QWidget>

PluginsController::PluginsController(QObject *parent)
    : QObject(parent)
{
}

void PluginsController::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.setSorting(QDir::Name);

#ifdef QT_DEBUG
    pluginsDir.cd("plugins");
#else
    pluginsDir.cd("../lib/dde-control-center/plugins");
#endif

    QStringList plugins = pluginsDir.entryList(QDir::Files);
    // pluginsDir.setSorting not working as expected on loongson.
    qSort(plugins.begin(), plugins.end());

    for (const QString file : plugins)
    {
        if (!QLibrary::isLibrary(file))
            continue;

        // load library
        QPluginLoader *pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(file), this);
        qDebug() << "load plugin: " << pluginLoader->metaData();

        PluginInterface *interface = qobject_cast<PluginInterface *>(pluginLoader->instance());
        if (!interface)
        {
            qWarning() << pluginLoader->errorString();
            pluginLoader->unload();
            pluginLoader->deleteLater();
            return;
        } else {
            qDebug() << "get plugin interface: " << interface;
        }

        interface->initialize(this);
        QWidget *w = interface->centralWidget();
        w->setVisible(false);
        Q_EMIT pluginAdded(w);
    }
}

void PluginsController::showModulePage(const QString &module, const QString &page, bool animation)
{
    Q_EMIT requestModulePage(module, page, animation);
}
