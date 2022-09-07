// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "resetpassworddialog.h"

#include <DApplication>
#include <QTranslator>
#include <DDBusSender>
#include <QDBusConnection>
#include <LogManager.h>
#include <csignal>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("XDG_CURRENT_DESKTOP")) {
        qputenv("XDG_CURRENT_DESKTOP", "Deepin");
    }

    DApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);
    a.setOrganizationName("deepin");

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption userNameOption(QStringList() << "u" << "username", "user name", "username", "");
    parser.addOption(userNameOption);
    QCommandLineOption appNameOption(QStringList() << "a" << "appname", "application name", "applicationName", "");
    parser.addOption(appNameOption);
    QCommandLineOption fdOption(QStringList() << "fd" << "filedescriptipn", "file descriptipn", "filedescriptipn", "");
    parser.addOption(fdOption);

    parser.parse(QCoreApplication::arguments());

    QString userName = parser.value(userNameOption);
    QString appName = parser.value(appNameOption);
    int fd = parser.value(fdOption).toInt();

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/reset-password-dialog_" + QLocale::system().name());
    QTranslator dccTranslator;
    dccTranslator.load("/usr/share/dde-control-center/translations/dde-control-center_" + QLocale::system().name());

    a.installTranslator(&translator);
    a.installTranslator(&dccTranslator);
    a.loadTranslator();

    DLogManager::setlogFilePath(QString("/tmp/%1.log").arg(a.applicationName()));
    const QDir &logDir = QFileInfo((Dtk::Core::DLogManager::getlogFilePath())).dir();
    if (!logDir.exists())
        QDir().mkpath(logDir.path());
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    Manager *manager = new Manager(userName, appName, fd);
    manager->start();

    std::signal(SIGTERM, &Manager::exit);
    std::signal(SIGKILL, &Manager::exit);

    return a.exec();
}
