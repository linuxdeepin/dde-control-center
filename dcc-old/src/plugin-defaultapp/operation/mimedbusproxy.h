//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

using ObjectInterfaceMap = QMap<QString, QVariantMap>;
using ObjectMap = QMap<QDBusObjectPath, ObjectInterfaceMap>;
using QStringMap = QMap<QString, QString>;
using PropMap = QMap<QString, QStringMap>;

Q_DECLARE_METATYPE(ObjectInterfaceMap)
Q_DECLARE_METATYPE(ObjectMap)
Q_DECLARE_METATYPE(QStringMap)
Q_DECLARE_METATYPE(PropMap)

class MimeDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MimeDBusProxy(QObject *parent = nullptr);

    QDBusPendingReply<void> SetDefaultApp(const QString &mime, const QString &desktopId);
    void DeleteApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteUserApp(const QString &desktopId);
    void AddUserApp(const QStringList &mimeTypes, const QString &desktopId);

    QDBusPendingReply<ObjectMap> GetManagedObjects();
    QDBusPendingReply<QString, QDBusObjectPath> GetDefaultApp(const QString &mimeType);
    QDBusPendingReply<ObjectMap> ListApps(const QString &mimeType);
    QString getAppId(const QDBusObjectPath &path);

Q_SIGNALS:
    void Change();

private:
    QDBusInterface *m_mimeInter;
    QDBusInterface *m_applicationManagerInter;
};
