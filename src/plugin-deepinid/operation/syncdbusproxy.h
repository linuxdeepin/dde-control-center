// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SYNCDBUSPROXY_H
#define SYNCDBUSPROXY_H

#include <DDBusInterface>

#include <QDBusMetaType>
#include <QObject>
#include <QtCore/QString>
#include <QtDBus/QtDBus>

struct IntString
{
    qint32 state;
    QString description;

    bool operator!=(const IntString &intString);
};

Q_DECLARE_METATYPE(IntString)

QDBusArgument &operator<<(QDBusArgument &argument, const IntString &intString);
const QDBusArgument &operator>>(const QDBusArgument &argument, IntString &intString);

void registerIntStringMetaType();

using Dtk::Core::DDBusInterface;

class SyncDBusProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong lastSyncTime READ lastSyncTime NOTIFY lastSyncTimeChanged)
    Q_PROPERTY(IntString state READ state NOTIFY stateChanged)

public:
    explicit SyncDBusProxy(QObject *parent = nullptr);

    void switcherSet(const QString &arg_0, bool state);
    bool switcherGet(const QString &arg_0);
    QDBusPendingCall switcherDump();

    qlonglong lastSyncTime();
    IntString state();

signals:
    void SwitcherChange(QString, bool);
    void lastSyncTimeChanged(qlonglong);
    void stateChanged(IntString);

private:
    DDBusInterface *m_syncInner;

};

#endif // SYNCDBUSPROXY_H
