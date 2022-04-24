/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <QObject>
class QDBusInterface;
class QDBusMessage;

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
    void Notify(const QString &in0, const uint in1, const QString &in2, const QString &in3, const QString &in4,
                const QStringList &in5, const QVariantMap &in6, const int in7);

Q_SIGNALS: // SIGNALS
    // deepin id
    void IsLoginChanged(const bool value);
    void DeviceUnlockedChanged(const bool value);
    // grub2
    void EnableThemeChanged(const bool value);
    void UpdatingChanged(const bool value);
    void DefaultEntryChanged(const QString &entry);
    void TimeoutChanged(const uint value);
    // grub2.EditAuth
    void EnabledUsersChanged(const QStringList &users);
    // grub2.Theme
    void BackgroundChanged(const QString &name);

    // license
    void AuthorizationStateChanged(const int code);
    void LicenseStateChanged(const int code);

    void DeepinIdError(const int code, const QString &msg);

    //reset signals
    void resetEnableTheme();
    void resetGrubEditAuthEnabled();

private Q_SLOTS:
    void onPropertiesChanged(const QDBusMessage &message);
    void BackgroundChangedPrivate();

private:
    QDBusInterface *m_grubInter;
    QDBusInterface *m_grubThemeInter;
    QDBusInterface *m_grubEditAuthInter;
    QDBusInterface *m_deepinIdInter;
    QDBusInterface *m_licenseInter;
    QDBusInterface *m_userexperienceInter;
    QDBusInterface *m_notificationInter;
};
