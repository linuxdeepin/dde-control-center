/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "reminderdialog.h"

#include <QApplication>
#include <QTranslator>
#include <QDBusConnection>

#define Service "com.deepin.dialogs.RebootReminder"
#define Path "/com/deepin/dialogs/RebootReminder"
#define Interface "com.deepin.dialogs.RebootReminder"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
        qWarning() << "there's an instance running, invoking Show method on it.";
        QProcess::startDetached("qdbus com.deepin.dialogs.RebootReminder"\
                                "/com/deepin/dialogs/RebootReminder"\
                                "com.deepin.dialogs.RebootReminder.Show");
    }
}
