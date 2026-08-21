// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SYNCDBUSPROXY_H
#define SYNCDBUSPROXY_H

#include <DDBusInterface>

#include <QDBusMetaType>
#include <QObject>
#include <QtCore/QString>
#include <QtDBus/QtDBus>

using Dtk::Core::DDBusInterface;

class SyncDBusProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong LastSyncTime READ lastSyncTime NOTIFY LastSyncTimeChanged)

public:
    explicit SyncDBusProxy(QObject *parent = nullptr);

    void SwitcherSet(const QString &arg_0, bool state);
    bool SwitcherGet(const QString &arg_0);
    QDBusPendingCall SwitcherDump();

    qlonglong lastSyncTime();

signals:
    void SwitcherChange(QString, bool);
    void LastSyncTimeChanged(qlonglong);

private:
    DDBusInterface *m_syncInter;

};

#endif // SYNCDBUSPROXY_H
