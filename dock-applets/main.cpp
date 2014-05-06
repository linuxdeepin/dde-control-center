#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickWindow>
#include <QtCore/QUrl>
#include <QDebug>
#include "resource.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<IconResource>("Resource", 1,0, "IconResource");

    QQmlEngine engine;
    QQmlComponent component(&engine);
    QQuickWindow::setDefaultAlphaBuffer(true);
    component.loadUrl(QUrl("qrc:///qml/main.qml"));
    if ( component.isReady() )
        component.create();
    else
        qWarning() << component.errorString();
    return app.exec();
}
