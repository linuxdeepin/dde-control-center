/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "resetpassworddialog.h"

#include <DApplication>
#include <QTranslator>
#include <DDBusSender>
#include <QDBusConnection>
#include <LogManager.h>

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

    parser.parse(QCoreApplication::arguments());

    QString userName = parser.value(userNameOption);
    QString appName = parser.value(appNameOption);

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/reset-password-dialog_" + QLocale::system().name());
    a.installTranslator(&translator);

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    DLogManager::setlogFilePath(QString("/tmp/%1.log").arg(a.applicationName()));
    const QDir &logDir = QFileInfo((Dtk::Core::DLogManager::getlogFilePath())).dir();
    if (!logDir.exists())
        QDir().mkpath(logDir.path());

    Manager *manager = new Manager(userName, appName);
    manager->start();

    return a.exec();
}
