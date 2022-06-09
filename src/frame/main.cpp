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

#include "controlcenterdbusadaptor.h"
#include "mainwindow.h"
#include "accessible.h"
#include "utils.h"

#include <DApplication>
#include <DDBusSender>
#include <DLog>
#include <DApplicationSettings>
#include <DApplicationHelper>

#include <QIcon>
#include <QScreen>

#include <qdebug.h>
#include <qfileinfo.h>
#include <qsettings.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

// 记录是否发生过崩溃，防止多线程崩溃重复记录崩溃信息
bool IsCrashed = false;

void sig_crash(int sig)
{
    if (IsCrashed)
        exit(1);
    // backtrace
    void *backtrace_buffer[100];
    auto size = backtrace(backtrace_buffer, 100);
    auto strings = backtrace_symbols(backtrace_buffer, size);

    // cache log
    QStringList crash_logs;
    qDebug() << "\ndde-control-center crashed! this is the log:";
    for (int i = 0; i < size; i++) {
        const auto &log = QLatin1String(strings[i]);
        crash_logs << log;
        qDebug() << log;
    }
    free(strings);

    // analyze log and save config
#ifdef QT_DEBUG
    const QString &pluginDirectory = "/home";
#else
    const QString &pluginDirectory = "/usr/lib/dde-control-center";
#endif
    for (auto &&log : crash_logs) {
        if (log.startsWith(pluginDirectory)) {
            const QString &pluginPath = log.left(log.indexOf(".so") + 3);
            qWarning() << "find crash plugin path:" << pluginPath;
            IsCrashed = true;

            QFileInfo fileInfo(pluginPath);
            QSettings settings(DCC_NAMESPACE::CollapseConfgPath, QSettings::IniFormat);
            settings.beginGroup("collapse");
            settings.setValue(fileInfo.fileName(), DCC_NAMESPACE::getFileMd5(pluginPath).toHex());
            settings.endGroup();

            DDBusSender()
                .service("org.freedesktop.Notifications")
                .path("/org/freedesktop/Notifications")
                .interface("org.freedesktop.Notifications")
                .method("Notify")
                .arg(QString())
                .arg((uint)0)
                .arg(QStringLiteral("dialog-warning"))
                .arg(QObject::tr("crashed"))
                .arg(QObject::tr("The plugin %1 crashed, next boot will not load, place reboot dde-control-center!").arg(pluginPath))
                .arg(QStringList())
                .arg(QVariantMap())
                .arg((int)5000)
                .call();
            break;
        }
    }

    exit(1);
}

int main(int argc, char *argv[])
{
    DApplication *app = DApplication::globalApplication(argc, argv);

    signal(SIGSEGV, sig_crash);
    signal(SIGILL,  sig_crash);
    signal(SIGABRT, sig_crash);
    signal(SIGFPE,  sig_crash);

    app->setOrganizationName("deepin");
    app->setApplicationName("dde-control-center");

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineOption dbusOption(QStringList() << "d" << "dbus" << "startup on dbus");
    QCommandLineOption pageOption("p", "specified module page", "page");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(dbusOption);
    parser.addOption(pageOption);
    parser.process(*app);

    const QString &reqPage = parser.value(pageOption);

    if (!app->setSingleInstance(app->applicationName())) {
        if (parser.isSet(toggleOption)) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Toggle")
            .call();
        }

        if (!reqPage.isEmpty()) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("ShowPage")
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
        verstr="6.0";
    app->setApplicationVersion(verstr);
#else
    app->setApplicationVersion("6.0");
#endif
    app->setAttribute(Qt::AA_UseHighDpiPixmaps);
    // app->loadTranslator();
    app->setStyle("chameleon");
    app->setProductIcon(QIcon::fromTheme("preferences-system"));
    app->setWindowIcon(QIcon::fromTheme("preferences-system"));

    DApplicationSettings settings;

    app->setApplicationDisplayName(QObject::tr("Control Center"));
    app->setApplicationDescription(QApplication::translate("main", "Control Center provides the options for system settings."));

    QAccessible::installFactory(accessibleFactory);

    pid_t pid = getpid();
    qApp->setProperty("AppPid", pid);

    DCC_NAMESPACE::MainWindow mw;

    DCC_NAMESPACE::ControlCenterDBusAdaptor adaptor(&mw);
    // DBusControlCenterGrandSearchService grandSearchadAptor(&mw);

    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
        !conn.registerObject("/com/deepin/dde/ControlCenter", &mw)) {
        qDebug() << "dbus service already registered!" << "pid is:" << pid;
        if (!parser.isSet(showOption))
            return -1;
    }

    if (!reqPage.isEmpty()) {
        adaptor.ShowPage(reqPage);
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
