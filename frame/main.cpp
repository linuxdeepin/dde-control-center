/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "frame.h"
#include "dbuscontrolcenterservice.h"

#include <DApplication>
#include <DLog>
#include <QStyle>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static const QString getQssFromFile(const QString &name)
{
#ifdef QT_DEBUG
//    qDebug() << "load qss file: " << name;
#endif

    QString qss;

    QFile f(name);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qss = f.readAll();
        f.close();
    }

    return std::move(qss);
}

static const QString getStyleSheetFromDir(QDir dir)
{
    QString ret;

    for (auto name : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
        if (name.suffix() == "qss") {
            ret.append(getQssFromFile(name.absoluteFilePath()));
        }
    }

    return std::move(ret);
}

static const QString styleSheetFromTheme(const QString &theme)
{
    QStringList moduleList = { "frame", "widgets", "accounts", "defapp",
                               "mouse", "wacom", "sound", "bluetooth",
                               "personalization", "datetime", "display",
                               "keyboard", "power", "systeminfo",  "update", "network"};

    QString ret;

    const QString resources = ":/%1/themes/" + theme;
    for (auto module : moduleList)
    {
        QString dir = resources.arg(module);
        ret.append(getStyleSheetFromDir(dir));
    }

    return std::move(ret);
}

static void onThemeChange(const QString &theme)
{
    QString qss;
    qss.append(styleSheetFromTheme("common"));
    qss.append(styleSheetFromTheme(theme));

#ifdef QT_DEBUG
//    qDebug() << "set theme:" << theme;
//    qDebug().noquote() << "qss: " << endl << qss;
#endif

    qApp->setStyleSheet(qss);
}

static void onFontSizeChanged(const float pointSizeF) {
    // TODO(hualet): DPI default to 96.
    auto PxToPt = [] (int px) -> float {
        return px * 72.0 / 96;
    };

    QFont font(qApp->font());
    if (pointSizeF <= 8.2) {
        font.setPointSizeF(PxToPt(10));
        qApp->setFont(font, "dcc::widgets::SmallLabel");
        font.setPointSizeF(PxToPt(11));
        qApp->setFont(font, "dcc::widgets::NormalLabel");
        font.setPointSizeF(PxToPt(12));
        qApp->setFont(font, "dcc::widgets::LargeLabel");
    } else if (pointSizeF == 9) {
        font.setPointSizeF(PxToPt(11));
        qApp->setFont(font, "dcc::widgets::SmallLabel");
        font.setPointSizeF(PxToPt(12));
        qApp->setFont(font, "dcc::widgets::NormalLabel");
        font.setPointSizeF(PxToPt(16));
        qApp->setFont(font, "dcc::widgets::LargeLabel");
    }  else if (pointSizeF == 9.7) {
        font.setPointSizeF(PxToPt(11));
        qApp->setFont(font, "dcc::widgets::SmallLabel");
        font.setPointSizeF(PxToPt(13));
        qApp->setFont(font, "dcc::widgets::NormalLabel");
        font.setPointSizeF(PxToPt(15));
        qApp->setFont(font, "dcc::widgets::LargeLabel");
    } else if (pointSizeF == 11.2) {
        font.setPointSizeF(PxToPt(12));
        qApp->setFont(font, "dcc::widgets::SmallLabel");
        font.setPointSizeF(PxToPt(15));
        qApp->setFont(font, "dcc::widgets::NormalLabel");
        font.setPointSizeF(PxToPt(18));
        qApp->setFont(font, "dcc::widgets::LargeLabel");
    } else if (pointSizeF >= 12) {
        font.setPointSizeF(PxToPt(14));
        qApp->setFont(font, "dcc::widgets::SmallLabel");
        font.setPointSizeF(PxToPt(16));
        qApp->setFont(font, "dcc::widgets::NormalLabel");
        font.setPointSizeF(PxToPt(20));
        qApp->setFont(font, "dcc::widgets::LargeLabel");
    }
}

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    DApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-control-center");
    app.setApplicationVersion("4.0");
    app.loadTranslator();
    app.setQuitOnLastWindowClosed(false);

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineOption moduleOption("m", "the module' id of which to be shown.", "module");
    QCommandLineOption pageOption("p", "specificed module page", "page");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(moduleOption);
    parser.addOption(pageOption);
    parser.process(app);

    const QString &reqModule = parser.value(moduleOption);
    const QString &reqPage = parser.value(pageOption);

    app.setTheme("semidark");
    onThemeChange("dark");

    QTimer::singleShot(0, [] { onFontSizeChanged(qApp->font().pointSizeF()); });

    Frame f;
    DBusControlCenterService adaptor(&f);
    Q_UNUSED(adaptor);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
        !conn.registerObject("/com/deepin/dde/ControlCenter", &f)) {
        qDebug() << "dbus service already registered!";

        if (parser.isSet(toggleOption))
            QProcess::startDetached("dbus-send --print-reply --dest=com.deepin.dde.ControlCenter /com/deepin/dde/ControlCenter com.deepin.dde.ControlCenter.Toggle");

        if (!reqModule.isEmpty())
            QProcess::startDetached(QString("dbus-send "
                                            "--print-reply "
                                            "--dest=com.deepin.dde.ControlCenter "
                                            "/com/deepin/dde/ControlCenter "
                                            "com.deepin.dde.ControlCenter.ShowPage "
                                            "\"string:%1\" \"string:%1\"").arg(reqModule).arg(reqPage));
        else if (parser.isSet(showOption))
            QProcess::startDetached("dbus-send "
                                    "--print-reply "
                                    "--dest=com.deepin.dde.ControlCenter "
                                    "/com/deepin/dde/ControlCenter "
                                    "com.deepin.dde.ControlCenter.Show");

#ifndef QT_DEBUG
        return 0;
#endif
    }

#ifndef QT_DEBUG
    if (parser.isSet(showOption) || parser.isSet(toggleOption))
#endif
        QTimer::singleShot(1, &f, &Frame::startup);

    if (!reqModule.isEmpty())
        QTimer::singleShot(10, &f, [&] { f.showSettingsPage(reqModule, reqPage); });

    return app.exec();
}
