// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
