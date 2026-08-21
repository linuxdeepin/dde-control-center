// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UTCLOUDDBUSPROXY_H
#define UTCLOUDDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;

class UtcloudDBusProxy : public QObject
{
    Q_OBJECT

public:
    explicit UtcloudDBusProxy(QObject *parent = nullptr);

    void SwitcherSet(const QString &arg_0, bool state);
    bool SwitcherGet(const QString &arg_0);
    QDBusPendingCall SwitcherDump();
    bool SetNickname(const QString &name, QString &errorMsg);

signals:
    void SwitcherChange(const QVariantList &);
    void LoginStatus(const QVariantList &);

private:
    QDBusInterface *m_utcloudInter;

};

#endif // UTCLOUDDBUSPROXY_H
