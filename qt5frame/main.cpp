/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QCoreApplication>
#include <QDBusConnection>
#include <qdebug.h>

#include "qmlloader.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads, true);
    QGuiApplication app(argc, argv);

    if(QDBusConnection::sessionBus().registerService(DBUS_NAME)){

        QmlLoader* qmlLoader = new QmlLoader();
        qmlLoader->rootContext->setContextProperty("mainObject", qmlLoader);
        qmlLoader->load(QUrl("qrc:///views/Main.qml"));
        QObject::connect(qmlLoader->engine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

        if(argc == 2){
            QString order = argv[1];
            if(order == "show"){
                qmlLoader->show();
            }
            else if(order == "toggle"){
                qmlLoader->toggle();
            }
            else {
                qmlLoader->showModule(order);
            }
        }

        return app.exec();
    } else {
        qWarning() << "dde control center is running...";
        return 0;
    }
}
