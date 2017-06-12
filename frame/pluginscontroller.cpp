#include "pluginscontroller.h"
#include "plugininterface.h"

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
        emit pluginAdded(w);
    }
}

void PluginsController::showModulePage(const QString &module, const QString &page)
{
    emit requestModulePage(module, page);
}
