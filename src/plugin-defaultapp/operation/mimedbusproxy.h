// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

    // 批量写入：后端接口本身接收 a{ss}，一次调用可写多个 MIME。
    // 每个 MIME 单独调用会逐次付出后端落盘成本（实测约 34ms/次），
    // Video 有 25 个 MIME，串行代价达数百毫秒。
    QDBusPendingReply<void> SetDefaultApp(const QStringMap &assignments);
    void DeleteApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteUserApp(const QString &desktopId);
    void AddUserApp(const QStringList &mimeTypes, const QString &desktopId);
    QDBusPendingReply<QString> addUserApplication(const QVariantMap &desktopFile, const QString &name);

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
