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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/dialogs_" + QLocale::system().name());
    a.installTranslator(&translator);

    Manager manager;
    manager.start();

    return a.exec();
}
