// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
