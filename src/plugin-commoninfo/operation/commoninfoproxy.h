//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusReply>
#include <QObject>
#include <DDBusInterface>

using Dtk::Core::DDBusInterface;

class CommonInfoProxy : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoProxy(QObject *parent = nullptr);

    // deepin id
    Q_PROPERTY(bool IsLogin READ IsLogin NOTIFY IsLoginChanged)
    bool IsLogin();
    Q_PROPERTY(bool DeviceUnlocked READ DeviceUnlocked NOTIFY DeviceUnlockedChanged)
    bool DeviceUnlocked();
    void UnlockDevice();
    void Login();

    // grub2
    QStringList GetSimpleEntryTitles();
    Q_PROPERTY(bool EnableTheme READ EnableTheme WRITE setEnableTheme NOTIFY EnableThemeChanged)
    bool EnableTheme();
    void setEnableTheme(const bool value);
    Q_PROPERTY(bool Updating READ Updating NOTIFY UpdatingChanged)
    bool Updating();
    Q_PROPERTY(QString DefaultEntry READ DefaultEntry WRITE setDefaultEntry NOTIFY DefaultEntryChanged)
    QString DefaultEntry();
    void setDefaultEntry(const QString &entry);
    Q_PROPERTY(uint Timeout READ Timeout WRITE setTimeout NOTIFY TimeoutChanged)
    uint Timeout();
    void setTimeout(const uint timeout);
    // grub2.EditAuth
    Q_PROPERTY(QStringList EnabledUsers READ EnabledUsers NOTIFY EnabledUsersChanged)
    QStringList EnabledUsers();
    void DisableUser(const QString &username);
    void EnableUser(const QString &username, const QString &password);
    // grub2.Theme
    Q_PROPERTY(QString Background READ Background WRITE setBackground NOTIFY BackgroundChanged)
    QString Background();
    void setBackground(const QString &name);

    // license
    Q_PROPERTY(int AuthorizationState READ AuthorizationState NOTIFY AuthorizationStateChanged)
    int AuthorizationState();
    Q_PROPERTY(int LicenseState READ LicenseState NOTIFY LicenseStateChanged)
    int LicenseState();

    // userexperience
    void Enable(const bool value);
    bool IsEnabled();

    // notification
    void Notify(const QString &inAppName, const uint replacesId, const QString &appIcon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, const int expireTimeout);
    // groubScale
    QDBusPendingCall SetScalePlymouth(int scale);

    Q_PROPERTY(bool DeveloperMode READ DeveloperMode NOTIFY DeveloperModeChanged)
    bool DeveloperMode();


Q_SIGNALS: // SIGNALS
    // deepin id
    void IsLoginChanged(const bool value);
    void DeviceUnlockedChanged(const bool value);
    void DeveloperModeChanged(const bool value);
    // grub2
    void EnableThemeChanged(const bool value);
    void UpdatingChanged(const bool value);
    void DefaultEntryChanged(const QString &entry);
    void TimeoutChanged(const uint value);
    // grub2.EditAuth
    void EnabledUsersChanged(const QStringList &users);
    // grub2.Theme
    void BackgroundChanged();


    // license
    void AuthorizationStateChanged(const int code);
    void LicenseStateChanged(const int code);

    void DeepinIdError(const int code, const QString &msg);

    // reset signals
    void resetEnableTheme();
    void resetGrubEditAuthEnabled();

private:
    DDBusInterface *m_grubInter;
    DDBusInterface *m_grubThemeInter;
    DDBusInterface *m_grubEditAuthInter;
    DDBusInterface *m_deepinIdInter;
    DDBusInterface *m_licenseInter;
    DDBusInterface *m_userexperienceInter;
    DDBusInterface *m_grubScaleInter;
    DDBusInterface *m_syncHelperInter;
};
