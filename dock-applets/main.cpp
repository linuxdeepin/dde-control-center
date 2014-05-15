#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QCoreApplication>
#include <QDBusConnection>

#include "helper.h"
#include "qmlloader.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads, true);
    QGuiApplication app(argc, argv);

    if(QDBusConnection::sessionBus().registerService("dde.dock.entry.AppletManager")){
        qmlRegisterType<ExternalObject>("Helper", 1,0, "ExternalObject");

        QmlLoader* qmlLoader = new QmlLoader();
        qmlLoader->rootContext->setContextProperty("mainObject", qmlLoader);
        qmlLoader->load(QUrl("qrc:///qml/main.qml"));

        return app.exec();
    } else {
        qWarning() << "is running...";
        return 0;
    }
}
