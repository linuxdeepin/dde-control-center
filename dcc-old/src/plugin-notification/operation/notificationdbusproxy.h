//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NotificationDBusProxy_H
#define NotificationDBusProxy_H

#include <QObject>
#include <QDBusPendingReply>
#include <QDBusReply>

class QDBusInterface;
class QDBusMessage;

class NotificationDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit NotificationDBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(QString allSetting READ allSetting WRITE setAllSetting NOTIFY allSettingChanged)
    QString allSetting();
    void setAllSetting(const QString &value);

    Q_PROPERTY(QString systemSetting READ systemSetting WRITE setSystemSetting NOTIFY systemSettingChanged)
    QString systemSetting();
    void setSystemSetting(const QString &value);

private:
    void init();

public Q_SLOTS: // METHODS
    //Notification
    QDBusPendingReply<> ClearRecords();
    QDBusPendingReply<> CloseNotification(uint in0);
    QDBusPendingReply<QString> GetAllRecords();
    QDBusPendingReply<QDBusVariant> GetAppInfo(const QString &in0, uint in1);
    QDBusPendingReply<QStringList> GetAppList();
    QDBusPendingReply<QStringList> GetCapbilities();
    QDBusPendingReply<QString> GetRecordById(const QString &in0);
    QDBusPendingReply<QString> GetRecordsFromId(int in0, const QString &in1);
    QDBusPendingReply<QString, QString, QString, QString> GetServerInformation();
    QDBusReply<QString> GetServerInformation(QString &out1, QString &out2, QString &out3);
    QDBusPendingReply<QDBusVariant> GetSystemInfo(uint in0);
    QDBusPendingReply<> Hide();
    QDBusPendingReply<uint> Notify(const QString &in0, uint in1, const QString &in2, const QString &in3, const QString &in4, const QStringList &in5, const QVariantMap &in6, int in7);
    QDBusPendingReply<> RemoveRecord(const QString &in0);
    QDBusPendingReply<> SetAppInfo(const QString &in0, uint in1, const QDBusVariant &in2);
    QDBusPendingReply<> SetSystemInfo(uint in0, const QDBusVariant &in1);
    QDBusPendingReply<> Show();
    QDBusPendingReply<> Toggle();
    QDBusPendingReply<QString> getAppSetting(const QString &in0);
    QDBusPendingReply<uint> recordCount();
    QDBusPendingReply<> setAppSetting(const QString &in0);

    void onPropertiesChanged(const QDBusMessage &message);

Q_SIGNALS: // SIGNALS
    // begin property changed signals
    void ActionInvoked(uint in0, const QString &in1);
    void AppAddedSignal(const QString &in0);
    void AppInfoChanged(const QString &in0, uint in1, const QDBusVariant &in2);
    void AppRemovedSignal(const QString &in0);
    void NotificationClosed(uint in0, uint in1);
    void RecordAdded(const QString &in0);
    void SystemInfoChanged(uint in0, const QDBusVariant &in1);
    void appAdded(const QString &in0);
    void appRemoved(const QString &in0);
    void appSettingChanged(const QString &in0);
    void systemSettingChanged(const QString &in0);
    // begin property changed signals
    void allSettingChanged(const QString & value) const;
    void systemSettingChanged(const QString & value) const;

private:
    QDBusInterface *m_dBusNotificationInter;
    QDBusInterface *m_dBusNotificationPropertiesInter;
};

#endif // NotificationDBusProxy_H
