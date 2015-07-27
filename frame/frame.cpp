#include "frame.h"
#include "homescreen.h"
#include "contentview.h"

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QPropertyAnimation>
#include <QDebug>

Frame::Frame(QWidget * parent) :
    QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    setFixedWidth(380);
    setFixedHeight(900);

    setStyleSheet("Frame { background-color: #252627 }");

    this->listPlugins();

    m_homeEffect = new QGraphicsOpacityEffect;
    m_homeEffect->setOpacity(1.0);
    m_contentEffect = new QGraphicsOpacityEffect;
    m_contentEffect->setOpacity(0.0);

    m_homeScreen = new HomeScreen(m_modules, this);
    m_homeScreen->setFixedWidth(this->width());
    m_homeScreen->setFixedHeight(this->height());
    m_homeScreen->setGraphicsEffect(m_homeEffect);

    m_contentView = new ContentView(m_modules, this);
    m_contentView->setFixedWidth(this->width());
    m_contentView->setFixedHeight(this->height());
    m_contentView->setGraphicsEffect(m_contentEffect);
    m_contentView->hide();

    m_homeScreen->setWindowOpacity(1.0);
    m_contentView->setWindowOpacity(1.0);

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

void Frame::switchToHome()
{
    QPropertyAnimation *homeAni = new QPropertyAnimation(m_homeEffect, "opacity");
    homeAni->setStartValue(0.0);
    homeAni->setEndValue(1.0);
    homeAni->setDuration(animationDuration);
    QPropertyAnimation *contentAni = new QPropertyAnimation(m_contentEffect, "opacity");
    contentAni->setStartValue(1.0);
    contentAni->setEndValue(0.0);
    contentAni->setDuration(animationDuration);

    connect(contentAni, &QPropertyAnimation::finished, [this] () -> void {m_contentView->hide();});

    homeAni->start();
    contentAni->start();

    m_homeScreen->show();
    m_contentView->show();
}

void Frame::switchToContent()
{
    QPropertyAnimation *homeAni = new QPropertyAnimation(m_homeEffect, "opacity");
    homeAni->setStartValue(1.0);
    homeAni->setEndValue(0.0);
    homeAni->setDuration(animationDuration);
    QPropertyAnimation *contentAni = new QPropertyAnimation(m_contentEffect, "opacity");
    contentAni->setStartValue(0.0);
    contentAni->setEndValue(1.0);
    contentAni->setDuration(animationDuration);

    connect(contentAni, &QPropertyAnimation::finished, [this] () -> void {m_homeScreen->hide();});

    homeAni->start();
    contentAni->start();

    m_homeScreen->show();
    m_contentView->show();
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
        switchToContent();
        //m_stackedLayout->setCurrentWidget(m_contentView);
    } else {
        switchToHome();
        //m_stackedLayout->setCurrentWidget(m_homeScreen);
    }
}
