#include "frame.h"
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QStackedLayout>

#include "homescreen.h"
#include "contentview.h"

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    setFixedWidth(380);
    setFixedHeight(900);

    setStyleSheet("Frame { background-color: #252627 }");

    this->listPlugins();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    m_homeScreen = new HomeScreen(m_modules, this);
    m_contentView = new ContentView(m_modules, this);

    m_stackedLayout = new QStackedLayout;
    m_stackedLayout->addWidget(m_homeScreen);
    m_stackedLayout->addWidget(m_contentView);

    mainLayout->addLayout(m_stackedLayout);

    connect(m_homeScreen, &HomeScreen::moduleSelected, this, &Frame::selectModule);
    connect(m_contentView, &ContentView::homeSelected, [=] { ModuleMetaData meta; this->selectModule(meta); });
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
        m_stackedLayout->setCurrentWidget(m_contentView);
    } else {
        m_stackedLayout->setCurrentWidget(m_homeScreen);
    }
}
