#include "frame.h"
#include "homescreen.h"
#include "contentview.h"
#include "constants.h"

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QDebug>

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    setWindowFlags(Qt::WindowStaysOnTopHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(DCC::ControlCenterWidth);
#ifdef QT_DEBUG
    setFixedHeight(800);
#endif
    setFixedHeight(qApp->desktop()->screenGeometry().height());

    setStyleSheet(QString("Frame { background-color:%1;}").arg(DCC::BgLightColor.name()));

    this->listPlugins();

    m_contentView = new ContentView(m_modules, this);
    m_contentView->setFixedWidth(this->width());
    m_contentView->setFixedHeight(this->height());

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedWidth(this->width());
    m_homeScreen->setFixedHeight(this->height());

    connect(m_homeScreen, &HomeScreen::moduleSelected, this, &Frame::selectModule);
    connect(m_contentView, &ContentView::homeSelected, [=] { ModuleMetaData meta; this->selectModule(meta);});
}

// override methods
void Frame::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    }
}

// private methods
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
    qWarning() << metaData.path;

    if (!metaData.path.isNull() && !metaData.path.isEmpty()) {
        m_contentView->setModule(metaData);
        m_contentView->show();
        m_homeScreen->hide();
    } else {
        m_homeScreen->show();
        m_contentView->hide();
    }
}
