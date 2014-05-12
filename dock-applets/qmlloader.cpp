
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QDBusConnection>
#include <QDebug>

#include "qmlloader.h"

QmlLoader::QmlLoader(QObject *parent)
    :QObject(parent)
{
    engine = new QQmlEngine(this);
    component = new QQmlComponent(engine, this);
    this->m_dbus_proxyer = new AppletDBus(this);
}

QmlLoader::~QmlLoader()
{

}


void QmlLoader::load(QUrl url)
{
    component->loadUrl(url);
    if ( component->isReady() )
        component->create();
    else
        qWarning() << component->errorString();
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
