/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "recoverydialog.h"

#include <DApplication>
#include <DLog>

#include <QTranslator>
#include <DDBusSender>
#include <QDBusConnection>
#include <QDebug>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    DApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/recovery_" + QLocale::system().name());
    a.installTranslator(&translator);

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    Manage *recovery = new Manage;
    if (QDBusConnection::sessionBus().registerService("com.deepin.ABRecovery"))
        QDBusConnection::sessionBus().registerObject("/com/deepin/ABRecovery", "com.deepin.ABRecovery", recovery, QDBusConnection::ExportAllSlots);

    if (QDBusConnection::sessionBus().registerService("com.deepin.SessionManager"))
        QDBusConnection::sessionBus().registerObject("/com/deepin/SessionManager", "com.deepin.SessionManager", recovery, QDBusConnection::ExportAllSlots);

    return a.exec();
}
