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

#include "dbuscontrolcenterservice.h"
#include "window/mainwindow.h"
#include "window/accessible.h"

#include <DApplication>
#include <DDBusSender>
#include <DLog>
#include <DApplicationSettings>

#include <QScreen>

#include <unistd.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static DCC_NAMESPACE::MainWindow *gwm{nullptr};

int main(int argc, char *argv[])
{
    DApplication *app = DApplication::globalApplication(argc, argv);
    if (!app->setSingleInstance(QString("dde-control-center_%1").arg(getuid()))) {
        qDebug() << "set single instance failed!";
        return -1;
    }
    app->setOrganizationName("deepin");
    app->setApplicationName("dde-control-center");

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineOption dbusOption(QStringList() << "d" << "dbus" << "startup on dbus");
    QCommandLineOption moduleOption("m", "the module' id of which to be shown.", "module");
    QCommandLineOption pageOption("p", "specified module page", "page");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(dbusOption);
    parser.addOption(moduleOption);
    parser.addOption(pageOption);
    parser.process(*app);

    const QString &reqModule = parser.value(moduleOption);
    const QString &reqPage = parser.value(pageOption);

    if (!app->setSingleInstance(app->applicationName(), DApplication::SingleScope::UserScope)) {
        if (parser.isSet(toggleOption)) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Toggle")
            .call();
        }

        if (!reqModule.isEmpty()) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("ShowPage")
            .arg(reqModule)
            .arg(reqPage)
            .call();
        }
        else if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Show")
            .call();
        }

        return -1;
    }

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

#ifdef CVERSION
    QString verstr(CVERSION);
    if (verstr.isEmpty())
        verstr="4.1";
    app->setApplicationVersion(CVERSION);
#else
    app->setApplicationVersion("4.0");
#endif
    app->setAttribute(Qt::AA_UseHighDpiPixmaps);
    app->loadTranslator();
    app->setStyle("chameleon");
    app->setProductIcon(QIcon::fromTheme("preferences-system"));
    app->setWindowIcon(QIcon::fromTheme("preferences-system"));

    DApplicationSettings settings;
    // load dde-network-utils translator
    QTranslator translator;
    translator.load("/usr/share/dde-network-utils/translations/dde-network-utils_" + QLocale::system().name());
    app->installTranslator(&translator);
    app->setApplicationDisplayName(QObject::tr("Control Center"));
    app->setApplicationDescription(QApplication::translate("main", "Control Center provides the options for system settings."));

    QAccessible::installFactory(accessibleFactory);

    QGSettings gs(ControlCenterGSettings, QByteArray(), app);
    auto w = gs.get(GSettinsWindowWidth).toInt();
    auto h = gs.get(GSettinsWindowHeight).toInt();
    pid_t pid = getpid();
    qDebug() << QString("main window size: %1 * %2").arg(w).arg(h) << ", pid is:" << pid;

    auto screen = app->primaryScreen();
    QRect mwRect(0, 0, w, h);
    mwRect.moveCenter(screen->geometry().center());

    DCC_NAMESPACE::MainWindow mw;
    mw.setGeometry(mwRect);
    gwm = &mw;

    DBusControlCenterService adaptor(&mw);

    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
        !conn.registerObject("/com/deepin/dde/ControlCenter", &mw)) {
        qDebug() << "dbus service already registered!" << "pid is:" << pid;
        return -1;
    }

    if (!reqModule.isEmpty()) {
        adaptor.ShowPage(reqModule, reqPage);
    }

    if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
        adaptor.Show();
    }

#ifdef QT_DEBUG
    //debug时会直接show
    //发布版本，不会直接显示，为了满足在被dbus调用时，
    //如果dbus参数错误，不会有任何UI上的变化
    if (1 == argc) {
        DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Show")
            .call();
    }
#endif

    return app->exec();
}
