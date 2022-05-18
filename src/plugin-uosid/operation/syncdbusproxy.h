/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYNCDBUSPROXY_H
#define SYNCDBUSPROXY_H

#include "interface/namespace.h"
#include "syncworker.h"
#include "intstring.h"
#include <QDBusInterface>
#include <QObject>

DCC_BEGIN_NAMESPACE
class DCCDBusInterface;
DCC_END_NAMESPACE

class BindCheckResult;
class SyncDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SyncDBusProxy(QObject *parent = nullptr);

public:
    void initConnent();
    // SYNC
    Q_PROPERTY(qlonglong LastSyncTime READ lastSyncTime NOTIFY LastSyncTimeChanged)
    qlonglong lastSyncTime();

    Q_PROPERTY(IntString State READ state NOTIFY StateChanged)
    IntString state();

    QString SwitcherDump();
    void SwitcherSet(const QString &in0, bool in1);

    // DEEPINID
    Q_PROPERTY(QVariantMap UserInfoDeepinid READ userInfo NOTIFY UserInfoDeepinidChanged)
    QVariantMap userInfo();

    QDBusPendingReply<QString> LocalBindCheck(const QString &uuid);
    void Login();
    void Logout();

    // SYNAHELPER
    QString UOSID();

    // hostname1
    QString StaticHostname();

signals:
    void SwitcherChange(const QString &switcher, bool enabled);
    // begin property changed signals
    void LastSyncTimeChanged(qlonglong  value) const;
    void StateChanged(IntString  value) const;
    void UserInfoDeepinidChanged(const QVariantMap & value) const;

private:
    DCC_NAMESPACE::DCCDBusInterface *m_syncInter;
    DCC_NAMESPACE::DCCDBusInterface *m_deepinId_inter;
    DCC_NAMESPACE::DCCDBusInterface *m_syncHelperInter;
    DCC_NAMESPACE::DCCDBusInterface *m_hostnameInter;
};

#endif // SYNCDBUSPROXY_H
