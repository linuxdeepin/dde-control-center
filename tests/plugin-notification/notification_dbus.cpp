/*
 * Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     dr <dongrui@uniontech.com
 *
 * Maintainer: dr <dongrui@uniontech.com
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/.
 */
#include "notification_dbus.h"

Notification_DBUS::Notification_DBUS(QObject *parent)
    : QObject(parent)
{

}

Notification_DBUS::~Notification_DBUS()
{

}

void Notification_DBUS::ClearRecords()
{

}

void Notification_DBUS::ClearRecordsQueued()
{

}

void Notification_DBUS::CloseNotification(uint)
{

}

void Notification_DBUS::CloseNotificationQueued(uint)
{

}

QString Notification_DBUS::GetAllRecords()
{
    return QString();
}

QDBusVariant Notification_DBUS::GetAppInfo(const QString &, uint index)
{
    switch (index) {
    case 0:
        return QDBusVariant("控制中心");
    case 1:
        return QDBusVariant("preferences-system");
    default:
        return QDBusVariant(true);
    }
}

QStringList Notification_DBUS::GetAppList()
{
    return {"test",
            "code",
            "deepin-editor",
            "deepin-font-manager",
            "org.gnome.Terminal",
            "electron-ssr",
            "dde-introduction",
            "deepin-app-store",
            "deepin-devicemanager",
            "deepin-boot-maker",
            "deepin-picker",
            "thunderbird",
            "kazam",
            "xtigervncviewer",
            "org.deepin.scaner",
            "deepin-manual",
            "deepin-screen-recorder",
            "google-chrome",
            "cnsetuputil2",
            "dde-computer",
            "designer-qt5",
            "deepin-reader",
            "com.sublimetext.3",
            "vmware-netcfg",
            "dde-printer",
            "deepin-movie",
            "deepin-image-viewer",
            "wps-office-wps",
            "health",
            "dde-file-manager",
            "jetbrains-goland",
            "org.gnome.Cheese",
            "wps-office-prometheus",
            "assistant-qt5",
            "deepin-voice-note",
            "wps-office-pdf",
            "GammaRay",
            "com.xunlei.download",
            "deepin-album",
            "wps-office-et",
            "ca.desrt.dconf-editor",
            "dde-calendar",
            "org.remmina.Remmina",
            "gparted",
            "vmware-workstation",
            "org.gnome.dfeet",
            "dde-trash",
            "deepin-contacts",
            "uos-service-support",
            "deepin-defender",
            "org.qt-project.qtcreator",
            "deepin-music",
            "deepin-system-monitor",
            "linguist-qt5",
            "wps-office-wpp",
            "deepin-deb-installer",
            "syntevo-smartgit",
            "deepin-mail",
            "deepin-log-viewer",
            "deepin-compressor",
            "deepin-calculator",
            "vmware-player",
            "chineseime-setting-wizard",
            "fcitx-config-gtk3",
            "deepin-draw",
            "deepin-terminal",
            "dde-control-center",
            "EasyConnect",
            "com.deekian.postman",
            "hyper",
            "com.oray.sunlogin.client",
            "org.kde.konsole",
            "rxvt-unicode",
            "dia",
            "vim",
            "gvim",
            "hotspot"};
}

QStringList Notification_DBUS::GetCapbilities()
{
    return QStringList();
}

QString Notification_DBUS::GetRecordById(const QString &)
{
    return QString();
}

QString Notification_DBUS::GetRecordsFromId(int, const QString &)
{
    return QString();
}

QString Notification_DBUS::GetServerInformation(QString &, QString &, QString &)
{
    return QString();
}

QDBusVariant Notification_DBUS::GetSystemInfo(uint)
{
    return QDBusVariant();
}

uint Notification_DBUS::Notify(const QString &, uint, const QString &, const QString &, const QString &, const QStringList &, const QVariantMap &, int)
{
    return uint();
}

void Notification_DBUS::RemoveRecord(const QString &)
{

}

void Notification_DBUS::RemoveRecordQueued(const QString &)
{

}

void Notification_DBUS::SetAppInfo(const QString &, uint, const QDBusVariant &)
{

}

void Notification_DBUS::SetAppInfoQueued(const QString &, uint, const QDBusVariant &)
{

}

void Notification_DBUS::SetSystemInfo(uint, const QDBusVariant &)
{

}

void Notification_DBUS::SetSystemInfoQueued(uint, const QDBusVariant &)
{

}

void Notification_DBUS::Toggle()
{

}

void Notification_DBUS::ToggleQueued()
{

}

QString Notification_DBUS::getAppSetting(const QString &)
{
    return QString();
}

uint Notification_DBUS::recordCount()
{
    return uint();
}

void Notification_DBUS::setAppSetting(const QString &)
{

}

void Notification_DBUS::setAppSettingQueued(const QString &)
{

}
