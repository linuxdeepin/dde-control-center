#ifndef QMLLOADER_H
#define QMLLOADER_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDBusAbstractAdaptor>

class QmlLoader;
class AppletDBus;

class QmlLoader: public QObject{
    Q_OBJECT

public:
    QmlLoader(QObject* parent = 0);
    ~QmlLoader();


    QQmlEngine* engine;
    QQmlComponent* component;
    QQmlContext * rootContext;

    void load(QUrl url);
private:
    AppletDBus * m_dbus_proxyer;
};

class AppletDBus : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dde.dock.entry.AppletManager")

public:
    AppletDBus(QmlLoader* parent);
    ~AppletDBus();

private:
    QmlLoader* m_parent;
};

#endif // QMLLOADER_H
