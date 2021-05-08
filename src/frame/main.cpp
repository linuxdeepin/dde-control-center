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
#include <QDir>

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

[[noreturn]] void sig_crash(int sig)
{
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0]);
    dir.cdUp();
    QString filePath = dir.path() + "/dde-collapse.log";
    qDebug() << "coredump log save path :" << filePath;
    QFile *file = new QFile(filePath);

    if (!file->open(QIODevice::Text | QIODevice::Append)) {
        qDebug() << file->errorString();
        exit(0);
    }

    if (file->size() >= 10 * 1024 * 1024) {
        // 清空原有内容
        file->close();
        if (file->open(QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << file->errorString();
            exit(0);
        }
    }

    // 捕获异常，打印崩溃日志到配置文件中
    try {
        QString head = "\n----" + qApp->applicationName() + "----\n"
                + QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]")
                + "[crash signal number:" + QString::number(sig) + "]\n";
        file->write(head.toUtf8());

#ifdef Q_OS_LINUX
        void *array[MAX_STACK_FRAMES];
        size_t size = 0;
        char **strings = nullptr;
        size_t i;
        signal(sig, SIG_DFL);
        size = static_cast<size_t>(backtrace(array, MAX_STACK_FRAMES));
        strings = backtrace_symbols(array, int(size));
        for (i = 0; i < size; ++i) {
            QString line = QString::number(i) + " " + QString::fromStdString(strings[i]) + "\n";
            file->write(line.toUtf8());

            std::string symbol(strings[i]);
            QString strSymbol = QString::fromStdString(symbol);
            int pos1 = strSymbol.indexOf("[");
            int pos2 = strSymbol.lastIndexOf("]");
            QString address = strSymbol.mid(pos1 + 1,pos2 - pos1 - 1);

            // 按照内存地址找到对应代码的行号
            QString cmd = "addr2line -C -f -e " + qApp->applicationName() + " " + address;
            QProcess *p = new QProcess;
            p->setReadChannel(QProcess::StandardOutput);
            p->start(cmd);
            p->waitForFinished();
            p->waitForReadyRead();
            file->write(p->readAllStandardOutput());
            delete p;
            p = nullptr;
        }
        free(strings);
#endif // __linux
    } catch (...) {
        //
    }
    file->close();
    delete file;
    file = nullptr;
    exit(0);
}


int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    if (!app.setSingleInstance(QString("dde-control-center_%1").arg(getuid()))) {
        qDebug() << "set single instance failed!";
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method("Show")
        .call();
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

    // 多屏情况下，当上一次保存的宽高比新的主屏大小还大，则用新屏幕的大小
    QRect screenRt = screen->geometry();
    if (w > screenRt.width())
        w = screenRt.width();
    if (h > screenRt.height())
        h = screenRt.height();

    QRect mwRect(0, 0, w, h);
    mwRect.moveCenter(screenRt.center());

    DCC_NAMESPACE::MainWindow mw;
    mw.setGeometry(mwRect);
    gwm = &mw;

    //崩溃信号
    signal(SIGTERM, sig_crash);
    signal(SIGSEGV, sig_crash);
    signal(SIGILL, sig_crash);
    signal(SIGINT, sig_crash);
    signal(SIGABRT, sig_crash);
    signal(SIGFPE, sig_crash);

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
