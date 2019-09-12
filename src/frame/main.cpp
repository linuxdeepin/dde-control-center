/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "frame.h"
#include "dbuscontrolcenterservice.h"
#include "window/mainwindow.h"

#include <DApplication>
#include <DDBusSender>
#include <DLog>
#include <DApplicationSettings>

#include <QScreen>
#include <QStyle>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const int MainWidgetWidget = 820;
const int MainWidgetHeight = 634;

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    DApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-control-center");
    app.setApplicationVersion("4.0");
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.loadTranslator();
    app.setTheme("light");
    app.setStyle("chameleon");

    DApplicationSettings settings;
    // load dde-network-utils translator
    QTranslator translator;
    translator.load("/usr/share/dde-network-utils/translations/dde-network-utils_" + QLocale::system().name());
    app.installTranslator(&translator);

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineOption moduleOption("m", "the module' id of which to be shown.", "module");
    QCommandLineOption pageOption("p", "specified module page", "page");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(moduleOption);
    parser.addOption(pageOption);
    parser.process(app);


    auto screen = app.primaryScreen();
    QRect mwRect(0, 0, MainWidgetWidget, MainWidgetHeight);
    mwRect.moveCenter(screen->geometry().center());

    DCC_NAMESPACE::MainWindow mw;
    mw.setGeometry(mwRect);
    mw.setMinimumSize(QSize(MainWidgetWidget, MainWidgetHeight));
    mw.show();

    const QString &reqModule = parser.value(moduleOption);
    const QString &reqPage = parser.value(pageOption);

    DBusControlCenterService adaptor(&mw);
    Q_UNUSED(adaptor);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
            !conn.registerObject("/com/deepin/dde/ControlCenter", &mw)) {
        qDebug() << "dbus service already registered!";

        if (parser.isSet(toggleOption))
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Toggle")
            .call();

        if (!reqModule.isEmpty())
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("ShowPage")
            .arg(reqModule)
            .arg(reqPage)
            .call();

        else if (parser.isSet(showOption))
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Show")
            .call();

#ifndef QT_DEBUG
        return 0;
#endif
    }

#ifndef QT_DEBUG
    if (parser.isSet(showOption) || parser.isSet(toggleOption))
#endif

    if (!reqModule.isEmpty())
        QTimer::singleShot(10, &mw, [&] { mw.showModulePage(reqModule, reqPage, false); });

    return app.exec();
}
