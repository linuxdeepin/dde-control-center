#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QCoreApplication>
#include <QDBusConnection>

#include "resource.h"
#include "qmlloader.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads, true);
    QGuiApplication app(argc, argv);

    if(QDBusConnection::sessionBus().registerService("dde.dock.entry.AppletManager")){
        qmlRegisterType<IconResource>("Resource", 1,0, "IconResource");

        QmlLoader* qmlLoader = new QmlLoader();
        qmlLoader->load(QUrl("qrc:///qml/main.qml"));

        return app.exec();
    } else {
        qWarning() << "is running...";
        return 0;
    }
}
