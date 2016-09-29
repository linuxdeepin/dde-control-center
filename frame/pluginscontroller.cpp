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

void PluginsController::pushWidget(QString mid, QWidget *w)
{
    Q_UNUSED(mid)
    Q_UNUSED(w)

    // TODO
}

void PluginsController::loadPlugins()
{
#ifdef QT_DEBUG
    const QDir pluginsDir("plugins");
#else
    const QDir pluginsDir("../lib/dde-dock/plugins");
#endif
    const QStringList plugins = pluginsDir.entryList(QDir::Files);

    for (const QString file : plugins)
    {
        if (!QLibrary::isLibrary(file))
            continue;

        // load library
        QPluginLoader *pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(file), this);
        PluginInterface *interface = qobject_cast<PluginInterface *>(pluginLoader->instance());
        if (!interface)
        {
            qDebug() << pluginLoader->errorString();
            pluginLoader->unload();
            pluginLoader->deleteLater();
            return;
        }

//        m_pluginList.insert(interface, QMap<QString, PluginsItem *>());
//        interface->init(this);
        interface->initialize(this);
        QWidget *w = interface->centeralWidget();
        w->setVisible(false);
        emit pluginAdded(w);
    }
}
