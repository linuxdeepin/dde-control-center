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
#include <QAccessible>

#include <stdio.h>
#include <time.h>
#include <execinfo.h>
#include <string>
#include <sys/stat.h>
#include <QStandardPaths>

#include <signal.h>
#include <unistd.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

static DCC_NAMESPACE::MainWindow *gwm{nullptr};

const int MAX_STACK_FRAMES = 128;

using namespace std;

void sig_crash(int sig)
{
    FILE *fd;
    struct stat buf;
    char path[100];
    memset(path, 0, 100);
    //崩溃日志路径
    QString strPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0] + "/dde-collapse.log";
    memcpy(path, strPath.toStdString().data(), static_cast<size_t>(strPath.length()));
    qDebug() << path;

    stat(path, &buf);
    if (buf.st_size > 10 * 1024 * 1024) {
        // 超过10兆则清空内容
        fd = fopen(path, "w");
    } else {
        fd = fopen(path, "at");
    }

    if (nullptr == fd) {
        exit(0);
    }
    //捕获异常，打印崩溃日志到配置文件中
    try {
        char szLine[512] = {0};
        time_t t = time(nullptr);
        tm *now = localtime(&t);
        sprintf(szLine, "#####dde-control-center#####\n[%04d-%02d-%02d %02d:%02d:%02d][crash signal number:%d]\n",
                            now->tm_year + 1900,
                            now->tm_mon + 1,
                            now->tm_mday,
                            now->tm_hour,
                            now->tm_min,
                            now->tm_sec,
                            sig);
        fwrite(szLine, 1, strlen(szLine), fd);
#ifdef __linux
        void *array[MAX_STACK_FRAMES];
        int size = 0;
        char **strings = nullptr;
        signal(sig, SIG_DFL);
        size = backtrace(array, MAX_STACK_FRAMES);
        strings = static_cast<char **>(backtrace_symbols(array, size));
        for (int i = 0; i < size; ++i) {
            char szLine[512] = {0};
            sprintf(szLine, "%d %s\n", i, strings[i]);
            fwrite(szLine, 1, strlen(szLine), fd);

            std::string symbol(strings[i]);

            size_t pos1 = symbol.find_first_of("[");
            size_t pos2 = symbol.find_last_of("]");
            std::string address = symbol.substr(pos1 + 1, pos2 - pos1 - 1);
            char cmd[128] = {0};
            sprintf(cmd, "addr2line -C -f -e dde-control-center %s", address.c_str()); // 打印当前进程的id和地址
            FILE *fPipe = popen(cmd, "r");
            if (fPipe != nullptr) {
                char buff[1024];
                memset(buff, 0, sizeof(buff));
                char *ret = fgets(buff, sizeof(buff), fPipe);
                pclose(fPipe);
                fwrite(ret, 1, strlen(ret), fd);
            }
        }
        free(strings);
#endif // __linux
    } catch (...) {
        //
    }
    fflush(fd);
    fclose(fd);
    fd = nullptr;
    exit(0);
}


int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();

    DApplication app(argc, argv);
    if (!app.setSingleInstance(QString("dde-control-center_%1").arg(getuid()))) {
        qDebug() << "set single instance failed!";
        return -1;
    }
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
    qDebug() << QString("main window size: %1 * %2").arg(w).arg(h) << ", pid is:" << pid;

    auto screen = app.primaryScreen();
    QRect mwRect(0, 0, w, h);
    mwRect.moveCenter(screen->geometry().center());

    DCC_NAMESPACE::MainWindow mw;
    mw.setGeometry(mwRect);
    gwm = &mw;

    //崩溃信号
    QGSettings setting("com.deepin.dde.control-center", QByteArray());
    if (setting.get("crash-signal-enable").toBool()) {
        signal(SIGTERM, sig_crash);
        signal(SIGSEGV, sig_crash);
        signal(SIGILL, sig_crash);
        signal(SIGINT, sig_crash);
        signal(SIGABRT, sig_crash);
        signal(SIGFPE, sig_crash);
    }

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

        if (!reqModule.isEmpty()) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("ShowPage")
            .arg(reqModule)
            .arg(reqPage)
            .call();
        } else if (parser.isSet(showOption)) {
            DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method("Show")
            .call();
        }
        // 当前服务注册失败，发送完dbus请求后退出，否则进程无法退出
        return 0;
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
