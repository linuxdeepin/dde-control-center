// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QDBusPendingReply>
class QDBusInterface;
class QDBusMessage;

class MimeDBusProxyOld : public QObject
{
    Q_OBJECT
public:
    static bool isRegisted();
    explicit MimeDBusProxyOld(QObject *parent = nullptr);

    QDBusPendingReply<void> SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteUserApp(const QString &desktopId);
    void AddUserApp(const QStringList &mimeTypes, const QString &desktopId);

    QString GetDefaultApp(const QString &mimeType);
    QString ListApps(const QString &mimeType);

    QString ListUserApps(const QString &mimeType);

Q_SIGNALS: // SIGNALS
    void Change();
    // begin property changed signals

private:
    QDBusInterface *m_mimeInter;
};

