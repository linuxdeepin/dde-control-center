// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "accessible.h"
#include "controlcenterdbusadaptor.h"
#include "mainwindow.h"
#include "utils.h"

#include <DApplication>
#include <DDBusSender>
#include <DGuiApplicationHelper>
#include <DLog>
#include <DIconTheme>

#include <QIcon>
#include <QScreen>
#include <QStringList>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

QStringList defaultpath()
{
    QStringList path;
#ifdef QT_DEBUG
    path.append(qApp->applicationDirPath());
#else
    path.append(DCC_NAMESPACE::PLUGIN_DIRECTORY);
    path.append(DCC_NAMESPACE::OLD_PLUGIN_DIRECTORY);
#endif
    return path;
}

int main(int argc, char *argv[])
{
    DApplication *app = DApplication::globalApplication(argc, argv);
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

    if (!app->setSingleInstance(app->applicationName())) {
        if (parser.isSet(toggleOption)) {
            DDBusSender()
                    .service("org.deepin.dde.ControlCenter1")
                    .interface("org.deepin.dde.ControlCenter1")
                    .path("/org/deepin/dde/ControlCenter1")
                    .method("Toggle")
                    .call();
        }

        if (!reqPage.isEmpty()) {
            DDBusSender()
                    .service("org.deepin.dde.ControlCenter1")
                    .interface("org.deepin.dde.ControlCenter1")
                    .path("/org/deepin/dde/ControlCenter1")
                    .method("ShowPage")
                    .arg(reqPage)
                    .call();
        } else if (parser.isSet(showOption) && !parser.isSet(dbusOption)) {
            DDBusSender()
                    .service("org.deepin.dde.ControlCenter1")
                    .interface("org.deepin.dde.ControlCenter1")
                    .path("/org/deepin/dde/ControlCenter1")
                    .method("Show")
                    .call();
        }

        return -1;
    }
    Dtk::Gui::DGuiApplicationHelper::loadTranslator(app->applicationName(), { TRANSLATE_READ_DIR }, { QLocale() });
    DLogManager::setLogFormat(
            "%{time}{yy-MM-ddTHH:mm:ss.zzz} [%{type}] [%{category}] <%{function}> %{message}");

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
    app->setAttribute(Qt::AA_UseHighDpiPixmaps);
    app->loadTranslator();
    app->setStyle("chameleon");
    app->setProductIcon(DIconTheme::findQIcon("preferences-system"));
    app->setWindowIcon(DIconTheme::findQIcon("preferences-system"));

    app->setApplicationDisplayName(QObject::tr("Control Center"));
    app->setApplicationDescription(
            QApplication::translate("main",
                                    "Control Center provides the options for system settings."));

    QAccessible::installFactory(accessibleFactory);

    DCC_NAMESPACE::MainWindow mw;

    DCC_NAMESPACE::ControlCenterDBusAdaptor adaptor(&mw);
    DCC_NAMESPACE::DBusControlCenterGrandSearchService grandSearchadAptor(&mw);

    if (!refPluginDir.isEmpty()) {
        mw.loadModules(true, { refPluginDir });
        QDBusConnection conn = QDBusConnection::sessionBus();
        if (!conn.registerService("org.deepin.dde.ControlCenter1")
            || !conn.registerObject("/org/deepin/dde/ControlCenter1", &mw)) {
            qDebug() << "dbus service already registered!"
                     << "pid is:" << qApp->applicationPid();
            if (!parser.isSet(showOption))
                return -1;
        }
        mw.show();
        return app->exec();
    }

    mw.loadModules(!parser.isSet(dbusOption), defaultpath());

    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("org.deepin.dde.ControlCenter1")
        || !conn.registerObject("/org/deepin/dde/ControlCenter1", &mw)) {
        qDebug() << "dbus service already registered!"
                 << "pid is:" << qApp->applicationPid();
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
    // debug时会直接show
    // 发布版本，不会直接显示，为了满足在被dbus调用时，
    // 如果dbus参数错误，不会有任何UI上的变化
    if (1 == argc) {
        DDBusSender()
                .service("org.deepin.dde.ControlCenter1")
                .interface("org.deepin.dde.ControlCenter1")
                .path("/org/deepin/dde/ControlCenter1")
                .method("Show")
                .call();
    }
#endif

    return app->exec();
}
