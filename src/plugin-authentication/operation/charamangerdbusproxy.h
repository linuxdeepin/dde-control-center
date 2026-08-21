//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHARAMANGERDBUSPROXY_H
#define CHARAMANGERDBUSPROXY_H

#include <QObject>
#include <QDBusObjectPath>
#include <QDBusUnixFileDescriptor>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;
class CharaMangerDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit CharaMangerDBusProxy(QObject *parent = nullptr);

    void setFingerprintInterTimeout(int timeout);

public:
    // CharaManger
    QString List(const QString &driverName, int charaType);
    QString driverInfo();
    QDBusPendingReply<QDBusUnixFileDescriptor> EnrollStart(const QString &driverName, int charaType, const QString &charaName);
    QDBusPendingReply<> EnrollStop();
    QDBusPendingReply<> Delete(int charaType, const QString &charaName);
    QDBusPendingReply<> Rename(int charaType, const QString &oldName, const QString &newName);

    // Fingerprint
    void setFingerprintTimeout(int timeout);
    QString defaultDevice();
    QDBusPendingReply<> Claim(const QString &username, bool claimed);
    QDBusPendingReply<> Enroll(const QString &finger);
    QStringList ListFingers(const QString &username);
    void StopEnroll();
    QDBusPendingReply<> DeleteFinger(const QString &username, const QString &finger);
    QDBusPendingReply<> RenameFinger(const QString &username, const QString &finger, const QString &newName);

signals:
    // CharaManger signals
    void CharaUpdated(const QString &charaName, int CharaType);
    void DriverChanged();
    void EnrollStatusCharaManger(const QString &Sender, int Code, const QString &Msg);
    void DriverInfoChanged(const QString & value) const;

    // Fingerprint signals
    void EnrollStatusFingerprint(const QString &id, int code, const QString &msg);
    void DefaultDeviceChanged(const QString &defaultDevice);
    void Touch(const QString &id, bool pressed);

    // SessionManager singnals
    void LockedChanged(bool  value) const;

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_charaMangerInter;
    QDBusInterface *m_fingerprintInter;
    QDBusInterface *m_SMInter;
};

#endif // CHARAMANGERDBUSPROXY_H
