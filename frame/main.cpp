/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/


#include <gtk/gtk.h>

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QTranslator>
#include <QCommandLineParser>

#include "frame.h"
#include "interfaces.h"
#include "logmanager.h"
#include "pluginsmanager.h"
#include "dbus/dbuscontrolcenter.h"
#include "dbus/dbuscontrolcenterservice.h"

#include <dapplication.h>
#include <dthememanager.h>

DWIDGET_USE_NAMESPACE

QString getQssFromFile(const QString &name)
{
    QString qss = "";

    QFile f(name);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qss = f.readAll();
        f.close();
    }

    return qss;
}

void onThemeChange(const QString &theme)
{
    QString fileName = QString(":/resources/qss/%1.qss").arg(theme);

    qApp->setStyleSheet(getQssFromFile(fileName));
}

int main(int argv, char *args[])
{
    gtk_init(&argv, &args);

    DApplication app(argv, args);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-control-center");
    app.setApplicationVersion("3.0");

    // keep running if all window hided
    qApp->setQuitOnLastWindowClosed(false);

#ifndef QT_DEBUG
    QDir::setCurrent(QApplication::applicationDirPath());
#endif

    // install translators
    QTranslator translator;
    translator.load("../share/dde-control-center/translations/dde-control-center_" + QLocale::system().name());
    app.installTranslator(&translator);

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addPositionalArgument("module", "the module's id of which to be shown.");
    parser.process(app);

    QStringList positionalArgs = parser.positionalArguments();

    // initialize logging
    LogManager::instance()->debug_log_console_on();

    Frame frame;
    DBusControlCenterService adaptor(&frame);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
            !conn.registerObject("/com/deepin/dde/ControlCenter", &frame)) {
        qDebug() << "dbus service already registered!";

        // call exist service
        DBusControlCenter c;

        if (parser.isSet(toggleOption)) {
            c.Toggle();
        }

        if (!positionalArgs.isEmpty()) {
            c.ShowModule(positionalArgs.at(0));
        } else if (parser.isSet(showOption)) {
            c.Show();
        }

#ifndef QT_DEBUG
        return 0;
#endif
    }

    if (!positionalArgs.isEmpty()) {

        PluginsManager *pluginsManager = PluginsManager::getInstance(&app);

        if (pluginsManager->pluginIndex(positionalArgs.at(0)) != -1) {
            frame.selectModule(positionalArgs.at(0));
        } else {
            frame.selectModule("home");
        }
    }
#ifndef QT_DEBUG
    else if (parser.isSet(showOption) || parser.isSet(toggleOption))
#endif

        frame.show();

    // setup theme manager
    DThemeManager *manager = DThemeManager::instance();
    QObject::connect(manager, &DThemeManager::themeChanged, [ = ](QString theme) {
        onThemeChange(theme);
    });

    manager->setTheme("dark");
    onThemeChange("dark");

    return app.exec();
}
