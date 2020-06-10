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
#include "window/accessible.h"

#include <DApplication>
#include <DDBusSender>
#include <DLog>
#include <DApplicationSettings>

#include <QScreen>
#include <QStyle>
#include <QGSettings>
#include <QAccessible>

#include <signal.h>
#include <unistd.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static DCC_NAMESPACE::MainWindow *gwm{nullptr};

void closeSignal(int s) {
    qDebug() << "signal 15!";
    if (gwm) {
        delete gwm;
    }
}

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    DApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-control-center");
#ifdef CVERSION
    QString verstr(CVERSION);
    if (verstr.isEmpty())
        verstr="4.1";
    app.setApplicationVersion(CVERSION);
#else
    app.setApplicationVersion("4.0");
#endif
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.loadTranslator();
    app.setStyle("chameleon");
    app.setProductIcon(QIcon::fromTheme("preferences-system"));
    app.setWindowIcon(QIcon::fromTheme("preferences-system"));

    DApplicationSettings settings;
    // load dde-network-utils translator
    QTranslator translator;
    translator.load("/usr/share/dde-network-utils/translations/dde-network-utils_" + QLocale::system().name());
    app.installTranslator(&translator);
    app.setApplicationDisplayName(QObject::tr("Control Center"));
    app.setApplicationDescription(QApplication::translate("main", "Control Center provides the options for system settings."));

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    QAccessible::installFactory(accessibleFactory);

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

    QGSettings gs(ControlCenterGSettings, QByteArray(), &app);
    auto w = gs.get(GSettinsWindowWidth).toInt();
    auto h = gs.get(GSettinsWindowHeight).toInt();
    pid_t pid = getpid();
    qDebug() << QString("main window size: %1 * %2").arg(w, h) << ", pid is:" << pid;

    auto screen = app.primaryScreen();
    QRect mwRect(0, 0, w, h);
    mwRect.moveCenter(screen->geometry().center());

    DCC_NAMESPACE::MainWindow mw;
    mw.setGeometry(mwRect);
    gwm = &mw;

    //处理SIGTERM 信号，保证在控制中心被强制关闭时，正常退出
    signal(15, closeSignal);

    const QString &reqModule = parser.value(moduleOption);
    const QString &reqPage = parser.value(pageOption);

    DBusControlCenterService adaptor(&mw);
    Q_UNUSED(adaptor);

    auto req = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.dde.ControlCenter");
    if (req.value()) {
        QDBusInterface inter("com.deepin.dde.ControlCenter",
                             "/com/deepin/dde/ControlCenter",
                             "com.deepin.dde.ControlCenter",
                             QDBusConnection::sessionBus(), nullptr);
        if (inter.isValid()) {
            qDebug() << "inter.isValid() call exitProc";
            inter.call("exitProc");
        } else {
            qDebug() << "inter isn't valid";
        }
    }

    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
            !conn.registerObject("/com/deepin/dde/ControlCenter", &mw)) {
        qDebug() << "dbus service already registered!" << "pid is:" << pid;

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
    }

    if (!reqModule.isEmpty())
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method("ShowPage")
        .arg(reqModule)
        .arg(reqPage)
        .call();

    if (parser.isSet(showOption))
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method("Show")
        .call();

#ifdef QT_DEBUG
    //debug时会直接show
    //发布版本，不会直接显示，为了满足在被dbus调用时，
    //如果dbus参数错误，不会有任何UI上的变化
    DDBusSender()
    .service("com.deepin.dde.ControlCenter")
    .interface("com.deepin.dde.ControlCenter")
    .path("/com/deepin/dde/ControlCenter")
    .method("Show")
    .call();
#endif

    return app.exec();
}
