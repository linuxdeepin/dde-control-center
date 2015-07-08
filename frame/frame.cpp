#include "frame.h"
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>

#include "homescreen.h"

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    setFixedWidth(380);
    setFixedHeight(900);

    setStyleSheet("Frame { background-color: #252627 }");

    this->loadPlugins();

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedSize(this->size());
}

// private methods
void Frame::loadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("modules");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!QLibrary::isLibrary(fileName))
            continue;

        QString filePath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader pluginLoader(filePath);
        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();

        ModuleMetaData meta = {
            filePath,
            metaData.value("name").toString(),
            metaData.value("icon").toObject().value("normal").toString(),
            metaData.value("icon").toObject().value("hover").toString(),
            metaData.value("icon").toObject().value("selected").toString()
        };

        m_modules << meta;
    }
}
