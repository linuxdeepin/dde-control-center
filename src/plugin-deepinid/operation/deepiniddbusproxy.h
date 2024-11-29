// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDDBUSPROXY_H
#define DEEPINIDDBUSPROXY_H

#include <DDBusInterface>
#include <QObject>
#include <QDBusReply>

using Dtk::Core::DDBusInterface;

class DeepinidDBusProxy: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap UserInfo READ userInfo NOTIFY UserInfoChanged)

public:
    explicit DeepinidDBusProxy(QObject *parent = nullptr);

    void login() const;
    void logout() const;

    QDBusReply<QString> localBindCheck(const QString &uuid);
    QDBusReply<QString> meteInfo();
    QVariantMap userInfo();

signals:
    void UserInfoChanged(const QVariantMap& value) const;

private:
    DDBusInterface *m_deepinId;
};

#endif // DEEPINIDDBUSPROXY_H
