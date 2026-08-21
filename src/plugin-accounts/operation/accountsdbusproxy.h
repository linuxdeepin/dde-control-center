//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCOUNTSDBUSPROXY_H
#define ACCOUNTSDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>
#include <DDBusInterface>

using Dtk::Core::DDBusInterface;

class QDBusMessage;

class AccountsDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit AccountsDBusProxy(QObject *parent = nullptr);

    // accounts
    Q_PROPERTY(QStringList UserList READ userList NOTIFY UserListChanged)
    QStringList userList();

    // groups
    Q_PROPERTY(QStringList GroupList READ groupList NOTIFY GroupListChanged)
    QStringList groupList();

    // displaymanager
    Q_PROPERTY(QList<QDBusObjectPath> Sessions READ sessions NOTIFY SessionsChanged)
    QList<QDBusObjectPath> sessions();

    Q_PROPERTY(bool QuickLoginEnabled READ quickLoginEnabled)
    bool quickLoginEnabled();

signals:
    void UserAdded(const QString &in0);
    void UserDeleted(const QString &in0);
    // begin property changed signals
    void UserListChanged(const QStringList &value) const;
    void GroupListChanged(const QStringList &value);

    // displaymanager
    void SessionsChanged(const QList<QDBusObjectPath> &value) const;

    void QuickLoginEnabledChanged(bool value);

public slots:
    QDBusPendingReply<QDBusObjectPath> CreateUser(const QString &in0, const QString &in1, int in2);
    QDBusPendingReply<> DeleteUser(const QString &in0, bool in1);
    QDBusPendingReply<QString> FindUserById(const qint64 &in0);
    QDBusPendingReply<QString> FindUserByName(const QString &in0);
    QDBusPendingReply<QStringList> GetGroups();
    QDBusPendingReply<QStringList> GetPresetGroups(int in0);
    QDBusPendingReply<bool, QString, int> IsPasswordValid(const QString &in0);
    QDBusPendingReply<bool, QString, int> IsUsernameValid(const QString &in0);
    QDBusPendingReply<QString> RandUserIcon();
    QDBusPendingReply<QString> GetGroupInfoByName(const QString &in0);
    QDBusPendingReply<> deleteGroup(const QString &in0);
    QDBusPendingReply<> createGroup(const QString &in0, uint32_t in1, bool in2);
    QDBusPendingReply<> modifyGroup(const QString &in0, const QString &in1, uint32_t in2);
    void onPropertiesChanged(const QDBusMessage &message);

private:
    void init();

private:
    DDBusInterface *m_dBusAccountsInter;
    DDBusInterface *m_dBusDisplayManagerInter;
};

#endif // ACCOUNTSDBUSPROXY_H
