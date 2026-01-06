// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "controlcenterdbusadaptor.h"
#include "dccmanager.h"

#include <DDBusSender>
#include <DIconTheme>
#include <DLog>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QStandardPaths>
#include <QStringList>
#include <QWindow>
#include <QDir>

DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

QString loggingRules(const QString &loggingModule)
{
    if (loggingModule.isEmpty())
        return "";

    static const QStringList levels = { "warning", "info", "debug" };
    const auto &list = loggingModule.split(",");
    const auto &moduleName = list.at(0);
    const auto &level = list.size() > 1 ? list.at(1) : "info";
    const int levelIndex = levels.indexOf(level);
    if (-1 == levelIndex)
        return "";

    QString rules = "*.debug=false\n*.info=false\n*.warning=false\n";
    for (int i = 0; i <= levelIndex; ++i) {
        auto rule = QString("org.deepin.dde.control-center.").append(moduleName).append(".").append(levels.at(i)).append("=true\n");
        rules.append(std::move(rule));
    }
    return rules;
}

QStringList defaultpath()
{
    QString pluginsDir(DefaultModuleDirectory);
    const QStringList path{ pluginsDir + "/plugins_v1.1", pluginsDir + "/plugins_v1.0" };
    // , QStringLiteral("/usr/lib/dde-control-center/modules/") };
    return path;
}

static void refreshQmlCache(const QString &version)
{
    if (version.isEmpty())
        return;
    static const QByteArray envCachePath = qgetenv("QML_DISK_CACHE_PATH");

    QString directory = envCachePath.isEmpty()
            ? QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                    + QLatin1String("/qmlcache/")
            : QString::fromLocal8Bit(envCachePath) + QLatin1String("/");

    QDir dir(directory);
    if (dir.exists()) {
        if (dir.exists(version))
            return;
        const auto ret = dir.removeRecursively();
        qDebug() << "Remove old QML cache:" << directory << "result:" << ret;
    }
    dir.mkpath(version);
}

int main(int argc, char *argv[])
{
    QGuiApplication *app = new QGuiApplication(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    app->setOrganizationName("deepin");
    app->setApplicationName("dde-control-center");
#ifdef CVERSION
    QString verstr(CVERSION);
    if (verstr.isEmpty())
        verstr = "6.0";
    app->setApplicationVersion(verstr);
#else
    app->setApplicationVersion("6.0");
#endif

    refreshQmlCache(app->applicationVersion());

    // take care of command line options
    QCommandLineOption showOption(QStringList() << "s" << "show", "show control center(hide for default).");
    QCommandLineOption toggleOption(QStringList() << "t" << "toggle", "toggle control center visible.");
    QCommandLineOption dbusOption(QStringList() << "d" << "dbus", "startup on dbus");
    QCommandLineOption moduleOption("m", "the module' id of which to be shown.", "module");
    QCommandLineOption pageOption("p", "specified module page", "page");
    QCommandLineOption showTime(QStringList() << "z" << "time", "show control center exe time."); // 新增time参数自动测试启动速度
    QCommandLineOption loggingModuleOption(QStringList() << "l" << "logging-module", "Only output logs for the specified module", "loggingModule");
    QCommandLineOption pluginDir("spec", "load plugins from specialdir", "plugindir");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Control Center");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(toggleOption);
    parser.addOption(dbusOption);
    parser.addOption(moduleOption);
    parser.addOption(pageOption);
    parser.addOption(showTime);
    parser.addOption(loggingModuleOption);
    parser.addOption(pluginDir);
    parser.process(*app);

    QString reqPage = parser.value(pageOption);
    const QString &reqModule = parser.value(moduleOption);
    if (!reqModule.isEmpty()) {
        reqPage = reqModule + "/" + reqPage;
    }
    const QStringList &refPluginDirs = parser.values(pluginDir);

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

    if (parser.isSet(loggingModuleOption)) {
        const auto &rules = loggingRules(parser.value(loggingModuleOption));
        if (!rules.isEmpty())
            QLoggingCategory::setFilterRules(rules);
    }

    DLogManager::setLogFormat("%{time}{yy-MM-ddTHH:mm:ss.zzz} [%{type}] [%{category}] <%{function}:%{line}> %{message}");

    DLogManager::registerJournalAppender();
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    auto dirs = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    QStringList fallbacks = QIcon::fallbackSearchPaths();
    for (const auto &fb : dirs) {
        fallbacks << fb + QLatin1String("/icons");
    }
    // To Fix Qt6 find icons will ignore ${GenericDataLocation}/icons/xxx
    QIcon::setFallbackSearchPaths(fallbacks);

    app->setWindowIcon(DIconTheme::findQIcon("preferences-system"));
    dccV25::DccManager::installTranslator("dde-control-center");
    app->setApplicationDisplayName(QObject::tr("Control Center"));
    // app->setApplicationDescription(QApplication::translate("main", "Control Center provides the options for system settings."));

    // QAccessible::installFactory(accessibleFactory);

    dccV25::DccManager *dccManager = new dccV25::DccManager(app);
    dccManager->init();
    QQmlApplicationEngine *engine = dccManager->engine();
    engine->loadFromModule("org.deepin.dcc", "DccWindow");
    QList<QObject *> objs = engine->rootObjects();
    for (auto &&obj : objs) {
        QWindow *w = qobject_cast<QWindow *>(obj);
        if (w) {
            dccManager->setMainWindow(w);
            break;
        }
    }
    if (!dccManager->mainWindow()) {
        qWarning() << "Failed to create window";
        return 1;
    }

    dccV25::ControlCenterDBusAdaptor *adaptor = new dccV25::ControlCenterDBusAdaptor(dccManager);
    dccV25::DBusControlCenterGrandSearchService *grandSearchadAptor = new dccV25::DBusControlCenterGrandSearchService(dccManager);
    Q_UNUSED(grandSearchadAptor)
    if (!conn.registerObject(DccDBusPath, dccManager)) {
        qDebug() << "dbus service already registered!" << "pid is:" << qApp->applicationPid();
        return -1;
    }
    if (!refPluginDirs.isEmpty()) {
        dccManager->loadModules(true, refPluginDirs);
        adaptor->Show();
    } else {
        dccManager->loadModules(!parser.isSet(dbusOption), defaultpath());
        if (!reqPage.isEmpty()) {
            adaptor->ShowPage(reqPage);
            adaptor->Show();
        } else if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
            adaptor->Show();
        } else if (parser.isSet(showTime)) {
            adaptor->Show();
            return 0;
        }

#ifdef QT_DEBUG
        // debug时会直接show
        // 发布版本，不会直接显示，为了满足在被dbus调用时，
        // 如果dbus参数错误，不会有任何UI上的变化
        if (1 == argc) {
            DDBusSender().service(DccDBusService).interface(DccDBusInterface).path(DccDBusPath).method("Show").call();
        }
#endif
    }
    int exitCode = app->exec();
    conn.unregisterService(DccDBusService);
#ifdef DCC_ENABLE_MEMORY_MANAGEMENT
    delete dccManager;
    delete app;
#endif
    return exitCode;
}
