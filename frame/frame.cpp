#include "frame.h"
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHBoxLayout>

#include "homescreen.h"

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    setFixedWidth(380);
    setFixedHeight(900);

    setStyleSheet("Frame { background-color: #252627 }");

    this->listPlugins();

    m_layout = new QHBoxLayout(this);

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedSize(this->size());

    connect(m_homeScreen, &HomeScreen::moduleSelected, this, &Frame::selectModule);
}

// override methods
void Frame::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    } else if (event->key() == Qt::Key_F1) {
        selectModule(ModuleMetaData { "" });
    }
}

// private methods
void Frame::loadPlugin(QString path)
{
    QPluginLoader loader(path);
    QObject * instance = loader.instance();
    if (instance) {
        ModuleInterface * module = qobject_cast<ModuleInterface*>(instance);
        m_layout->addWidget(module->getContent());
    }
}

void Frame::listPlugins()
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


// private slots
void Frame::selectModule(ModuleMetaData metaData)
{
    if (!metaData.path.isEmpty()) {
        this->loadPlugin(metaData.path);
        m_homeScreen->setVisible(false);
    } else {
        m_homeScreen->setVisible(true);
    }
}
