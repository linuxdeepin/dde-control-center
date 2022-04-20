/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     Tinalu Shao <shaotianlu@uniontech.com>
*
* Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
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
#ifndef ACCOUNTSDBUSPROXY_H
#define ACCOUNTSDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

class AccountsDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit AccountsDBusProxy(QObject *parent = nullptr);

    //accounts
    Q_PROPERTY(bool AllowGuest READ allowGuest NOTIFY AllowGuestChanged)
    bool allowGuest();

    Q_PROPERTY(QString GuestIcon READ guestIcon NOTIFY GuestIconChanged)
    QString guestIcon();

    Q_PROPERTY(QStringList UserList READ userList NOTIFY UserListChanged)
    QStringList userList();

    //displaymanager
    Q_PROPERTY(QList<QDBusObjectPath> Seats READ seats NOTIFY SeatsChanged)
    QList<QDBusObjectPath> seats();

    Q_PROPERTY(QList<QDBusObjectPath> Sessions READ sessions NOTIFY SessionsChanged)
    QList<QDBusObjectPath> sessions();

signals:
    void UserAdded(const QString &in0);
    void UserDeleted(const QString &in0);
    // begin property changed signals
    void AllowGuestChanged(bool  value) const;
    void GuestIconChanged(const QString & value) const;
    void UserListChanged(const QStringList & value) const;

    //displaymanager
    void SeatsChanged(const QList<QDBusObjectPath> & value) const;
    void SessionsChanged(const QList<QDBusObjectPath> & value) const;


public slots:
    QDBusPendingReply<> AllowGuestAccount(bool in0);
    QDBusPendingReply<QString> CreateGuestAccount();
    QDBusPendingReply<QDBusObjectPath> CreateUser(const QString &in0, const QString &in1, int in2);
    QDBusPendingReply<> DeleteUser(const QString &in0, bool in1);
    QDBusPendingReply<QString> FindUserById(const qint64 &in0);
    QDBusPendingReply<QString> FindUserByName(const QString &in0);
    QDBusPendingReply<QStringList> GetGroups();
    QDBusPendingReply<QStringList> GetPresetGroups(int in0);
    QDBusPendingReply<bool, QString, int> IsPasswordValid(const QString &in0);
    QDBusPendingReply<bool, QString, int> IsUsernameValid(const QString &in0);
    QDBusPendingReply<QString> RandUserIcon();


private slots:
    void onPropertiesChanged(const QDBusMessage &message);
    void onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w);
private:
    void init();

private:
    QDBusInterface *m_dBusAccountsInter;
    QDBusInterface *m_dBusDisplayManagerInter;
};

#endif // ACCOUNTSDBUSPROXY_H
