//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MIMEDBUSPROXY_H
#define MIMEDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>
class QDBusInterface;
class QDBusMessage;

class MimeDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MimeDBusProxy(QObject *parent = nullptr);

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

#endif // MIMEDBUSPROXY_H
