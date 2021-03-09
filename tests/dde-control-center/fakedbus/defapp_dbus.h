/*
 * Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     dr <dongrui@uniontech.com>
 *
 * Maintainer: dr <dongrui@uniontech.com>
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
#ifndef DEDAPP_DBUS_H
#define DEDAPP_DBUS_H

#include <QDBusContext>
#include <QObject>
#include <QDBusInterface>
#include <QDBusPendingReply>

#define DEFAPP_SERVICE_NAME "com.deepin.daemon.Mime"
#define DEFAPP_SERVICE_PATH "/com/deepin/daemon/Mime"

class Defapp_DBUS : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DEFAPP_SERVICE_NAME)

public:
    Defapp_DBUS(QObject *parent = nullptr);
    virtual ~Defapp_DBUS();

public Q_SLOTS: // METHODS
    void AddUserApp(const QStringList &mimeTypes, const QString &desktopId);

    void AddUserAppQueued(const QStringList &mimeTypes, const QString &desktopId);

    void DeleteApp(const QStringList &mimeTypes, const QString &desktopId);

    void DeleteAppQueued(const QStringList &mimeTypes, const QString &desktopId);

    void DeleteUserApp(const QString &desktopId);

    void DeleteUserAppQueued(const QString &desktopId);

    QString GetDefaultApp(const QString &mimeType);

    QString ListApps(const QString &mimeType);

    QString ListUserApps(const QString &mimeType);

    void SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId);

    void SetDefaultAppQueued(const QStringList &mimeTypes, const QString &desktopId);

Q_SIGNALS: // SIGNALS
    void Change();
    // begin property changed signals
};

#endif
