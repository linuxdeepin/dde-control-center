// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "reminderdialog.h"

#include <DApplication>
#include <QTranslator>
#include <DDBusSender>
#include <QDBusConnection>

#define Service "com.deepin.dialogs.RebootReminder"
#define Path "/com/deepin/dialogs/RebootReminder"
#define Interface "com.deepin.dialogs.RebootReminder"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/dialogs_" + QLocale::system().name());
    a.installTranslator(&translator);

    bool res = QDBusConnection::sessionBus().registerService(Service);
    if (res) {
        Manager *manager = new Manager;

        QDBusConnection::sessionBus().registerObject(Path, Interface, manager, QDBusConnection::ExportAllSlots);

        manager->start();

        return a.exec();

    } else {
        qDebug() << "there's an instance running, invoking Show method on it.";

        DDBusSender()
                .service("com.deepin.dialogs.RebootReminder")
                .interface("com.deepin.dialogs.RebootReminder")
                .path("/com/deepin/dialogs/RebootReminder")
                .method("Show")
                .call();
    }
}
