#include "frame.h"
#include <QDir>
#include <QPluginLoader>
#include <QtWidgets>

Frame::Frame()
{
    setFixedWidth(100);
    setFixedHeight(100);

    QGridLayout *layout = new QGridLayout(this);

    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("modules");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            ModuleInterface* moduleInterface = qobject_cast<ModuleInterface*>(plugin);
            layout->addWidget(moduleInterface->getContent());
            pluginLoader.unload();
        }
    }
}
