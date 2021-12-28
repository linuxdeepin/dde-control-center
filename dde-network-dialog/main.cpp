/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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

#include "localclient.h"
#include "utils.h"
#include "thememanager.h"
#include "networkcontroller.h"

#include <DApplication>
#include <DPlatformTheme>
#include <DLog>

#include <QWindow>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTranslator>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>

using namespace Dtk::Widget;

int main(int argc, char **argv)
{
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }
    DApplication *app = DApplication::globalApplication(argc, argv);
    app->setOrganizationName("deepin");
    app->setApplicationName("dde-network-dialog");
    app->setQuitOnLastWindowClosed(true);
    app->setAttribute(Qt::AA_UseHighDpiPixmaps);

    QTranslator translator;
    translator.load("/usr/share/dde-network-dialog/translations/dde-network-dialog_" + QLocale::system().name());
    qApp->installTranslator(&translator);

    qApp->setApplicationDisplayName("NetworkDialog");
    qApp->setApplicationDescription("network dialog");

    QCommandLineOption showOption(QStringList() << "s", "show config", "config");
    QCommandLineOption waitOption(QStringList() << "w", "wait wep-key"); // 等待模式，密码输入后返回给调用者并退出。否则不退出尝试联网
    QCommandLineOption connectPathOption(QStringList() << "c", "connect wireless ", "path");
    QCommandLineOption devOption(QStringList() << "n", "network device", "device");

    QCommandLineParser parser;
    parser.setApplicationDescription("DDE Network Dialog");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(showOption);
    parser.addOption(waitOption);
    parser.addOption(connectPathOption);
    parser.addOption(devOption);
    parser.process(*qApp);

    if (parser.isSet(showOption)) {
        QString config = parser.value(showOption);
        LocalClient::instance()->showPosition(nullptr, config.toUtf8());
    }
    if (parser.isSet(devOption)) {
        QString dev = parser.value(devOption);
        QString ssid = parser.value(connectPathOption);
        LocalClient::instance()->waitPassword(dev, ssid, parser.isSet(waitOption));
    } else {
        LocalClient::instance()->showWidget();
    }
    return app->exec();
}
