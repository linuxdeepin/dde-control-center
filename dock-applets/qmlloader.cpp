
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDBusConnection>
#include <QProcess>
#include <QDebug>

#include "qmlloader.h"

QmlLoader::QmlLoader(QObject *parent)
    :QObject(parent)
{
    engine = new QQmlEngine(this);
    component = new QQmlComponent(engine, this);
    rootContext = new QQmlContext(engine, this);
    this->m_dbus_proxyer = new AppletDBus(this);
}

QmlLoader::~QmlLoader()
{
    delete this->m_dbus_proxyer;
    delete this->rootContext;
    delete this->component;
    delete this->engine;
}


void QmlLoader::load(QUrl url)
{
    component->loadUrl(url);
    if ( component->isReady() )
        component->create();
    else
        qWarning() << component->errorString();
}

void QmlLoader::xdgOpen(QString path)
{
    QProcess::execute("xdg-open " + path);
}

AppletDBus::AppletDBus(QmlLoader *parent):
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    QDBusConnection::sessionBus().registerObject("/dde/dock/entry/AppletManager", parent);
}

AppletDBus::~AppletDBus()
{

}
