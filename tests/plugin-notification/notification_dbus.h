//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NOTIFICATION_DBUS_H
#define NOTIFICATION_DBUS_H

#include <QDBusContext>
#include <QObject>
#include <QDBusInterface>
#include <QJsonDocument>

#define NOTIFICATION_SERVICE_NAME "org.deepin.dde.Notification1"
#define NOTIFICATION_SERVICE_PATH "/org/deepin/dde/Notification1"

class Notification_DBUS : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", NOTIFICATION_SERVICE_NAME)

public:
    Notification_DBUS(QObject *parent = nullptr);
    virtual ~Notification_DBUS();

    Q_PROPERTY(QString allSetting READ allSetting WRITE setAllSetting NOTIFY AllSettingChanged)
    QString allSetting() { return m_allSetting; }
    void setAllSetting(const QString &value)
    {
        m_allSetting = value;
    }

    Q_PROPERTY(QString systemSetting READ systemSetting WRITE setSystemSetting NOTIFY SystemSettingChanged)
    QString systemSetting() {return m_systemSetting; }
    void setSystemSetting(const QString &value)
    {
        m_systemSetting = value;
    }

Q_SIGNALS: // SIGNALS
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
    void AllSettingChanged(const QString &value) const;
    void SystemSettingChanged(const QString &value) const;

public Q_SLOTS:
    void ClearRecords();
    void ClearRecordsQueued();
    void CloseNotification(uint);
    void CloseNotificationQueued(uint);
    QString GetAllRecords();
    QDBusVariant GetAppInfo(const QString &, uint index);
    QStringList GetAppList();
    QStringList GetCapbilities();
    QString GetRecordById(const QString &);
    QString GetRecordsFromId(int, const QString &);
    QString GetServerInformation(QString &, QString &, QString &);
    QDBusVariant GetSystemInfo(uint);
    uint Notify(const QString &, uint, const QString &, const QString &, const QString &, const QStringList &, const QVariantMap &, int);
    void RemoveRecord(const QString &);
    void RemoveRecordQueued(const QString &);
    void SetAppInfo(const QString &, uint, const QDBusVariant &);
    void SetAppInfoQueued(const QString &, uint, const QDBusVariant &);
    void SetSystemInfo(uint, const QDBusVariant &);
    void SetSystemInfoQueued(uint, const QDBusVariant &);
    void Toggle();
    void ToggleQueued();
    QString getAppSetting(const QString &);
    uint recordCount();
    void setAppSetting(const QString &);
    void setAppSettingQueued(const QString &);

private:
    QString m_allSetting;
    QString m_systemSetting;
};

#endif
