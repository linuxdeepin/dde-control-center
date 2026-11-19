// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QDBusPendingReply>
class QDBusInterface;
class QDBusMessage;
// 旧接口的读取结果。旧方法直接返回 QString 会把 DBus 错误退化成空串，
// 使调用方无法区分"读取失败"和"已确认未设置"。
struct LegacyReadReply {
    bool ok = false;
    QString value;
};

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

    // 保持原有同步读取语义，但把成功与否显式返回
    LegacyReadReply GetDefaultApp(const QString &mimeType);
    LegacyReadReply ListApps(const QString &mimeType);
    LegacyReadReply ListUserApps(const QString &mimeType);

Q_SIGNALS: // SIGNALS
    void Change();
    // begin property changed signals

private:
    QDBusInterface *m_mimeInter;
};

