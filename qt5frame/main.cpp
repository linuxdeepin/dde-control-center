#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickWindow>
#include <QtCore/QUrl>
#include <QDebug>
#include <QCoreApplication>
#include "popup.h"
#include <QQmlEngine>

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads, true);
    QGuiApplication app(argc, argv);

    qmlRegisterType<PopupItem>("DGui", 1,0, "PopupItem");

    QQmlEngine engine;
    QQmlComponent component(&engine);
    QQuickWindow::setDefaultAlphaBuffer(true);
    component.loadUrl(QUrl("qrc:///views/Main.qml"));
    if ( component.isReady() )
        component.create();
    else
        qWarning() << component.errorString();
    return app.exec();
}
