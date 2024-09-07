// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "controlcenterdbusadaptor.h"
#include "dccmanager.h"

#include <DDBusSender>
#include <DGuiApplicationHelper>
#include <DIconTheme>
#include <DLog>
#include <DPalette>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QStringList>
#include <QWindow>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

QStringList defaultpath()
{
    const QStringList path{ QStringLiteral(DefaultPluginsDirectory) };
    // , QStringLiteral("/usr/lib/dde-control-center/modules/") };
    return path;
}

int main(int argc, char *argv[])
{
    DGuiApplicationHelper::setAttribute(DGuiApplicationHelper::DontSaveApplicationTheme, true);
    DGuiApplicationHelper::setAttribute(DGuiApplicationHelper::UseInactiveColorGroup, false);

    QGuiApplication *app = new QGuiApplication(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    app->setOrganizationName("deepin");
    app->setApplicationName("dde-control-center");

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s"
                                                << "show",
                                  "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t"
                                                  << "toggle",
                                    "toggle control center visible.");
    QCommandLineOption dbusOption(QStringList() << "d"
                                                << "dbus",
                                  "startup on dbus");
    QCommandLineOption pageOption("p", "specified module page", "page");
    QCommandLineOption pluginDir("spec", "load plugins from specialdir", "plugindir");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(dbusOption);
    parser.addOption(pageOption);
    parser.addOption(pluginDir);
    parser.process(*app);

    const QString &reqPage = parser.value(pageOption);
    const QString &refPluginDir = parser.value(pluginDir);

    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService(DccDBusService)) {
        qDebug() << "dbus service already registered!"
                 << "pid is:" << qApp->applicationPid();
        if (parser.isSet(toggleOption)) {
            DDBusSender().service(DccDBusService).interface(DccDBusInterface).path(DccDBusPath).method("Toggle").call();
        }

        if (!reqPage.isEmpty()) {
            DDBusSender().service(DccDBusService).interface(DccDBusInterface).path(DccDBusPath).method("ShowPage").arg(reqPage).call();
        } else if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
            DDBusSender().service(DccDBusService).interface(DccDBusInterface).path(DccDBusPath).method("Show").call();
        }

        return -1;
    }

    DLogManager::setLogFormat("%{time}{yy-MM-ddTHH:mm:ss.zzz} [%{type}] [%{category}] <%{function}> %{message}");

    DLogManager::registerJournalAppender();
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

#ifdef CVERSION
    QString verstr(CVERSION);
    if (verstr.isEmpty())
        verstr = "6.0";
    app->setApplicationVersion(verstr);
#else
    app->setApplicationVersion("6.0");
#endif
    app->setWindowIcon(DIconTheme::findQIcon("preferences-system"));
    dccV25::DccManager::installTranslator("dde-control-center");
    app->setApplicationDisplayName(QObject::tr("Control Center"));
    // app->setApplicationDescription(QApplication::translate("main", "Control Center provides the options for system settings."));

    // QAccessible::installFactory(accessibleFactory);

    dccV25::DccManager *dccManager = new dccV25::DccManager();
    dccManager->init();
    QQmlApplicationEngine *engine = dccManager->engine();
    engine->load(QUrl(QStringLiteral(DefaultModuleDirectory) + "/DccWindow.qml"));
    QList<QObject *> objs = engine->rootObjects();
    for (auto &&obj : objs) {
        QWindow *w = qobject_cast<QWindow *>(obj);
        if (w) {
            dccManager->setMainWindow(w);
            break;
        }
    }
    if (!dccManager->mainWindow()) {
        qWarning() << "";
        return 1;
    }

    dccV25::ControlCenterDBusAdaptor *adaptor = new dccV25::ControlCenterDBusAdaptor(dccManager);
    dccV25::DBusControlCenterGrandSearchService *grandSearchadAptor = new dccV25::DBusControlCenterGrandSearchService(dccManager);

    if (!refPluginDir.isEmpty()) {
        dccManager->loadModules(true, { refPluginDir });
        QDBusConnection conn = QDBusConnection::sessionBus();
        if (!conn.registerObject(DccDBusPath, dccManager)) {
            qDebug() << "dbus service already registered!"
                     << "pid is:" << qApp->applicationPid();
            if (!parser.isSet(showOption))
                return -1;
        }
        adaptor->Show();
        return app->exec();
    }

    dccManager->loadModules(!parser.isSet(dbusOption), defaultpath());

    // QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerObject(DccDBusPath, dccManager)) {
        qDebug() << "dbus service already registered!"
                 << "pid is:" << qApp->applicationPid();
        if (!parser.isSet(showOption))
            return -1;
    }

    if (!reqPage.isEmpty()) {
        adaptor->ShowPage(reqPage);
    }

    if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
        adaptor->Show();
    }

#ifdef QT_DEBUG
    // debug时会直接show
    // 发布版本，不会直接显示，为了满足在被dbus调用时，
    // 如果dbus参数错误，不会有任何UI上的变化
    if (1 == argc) {
        DDBusSender().service(DccDBusService).interface(DccDBusInterface).path(DccDBusPath).method("Show").call();
    }
#endif
    int exitCode = app->exec();
    delete dccManager;
    return exitCode;
}
