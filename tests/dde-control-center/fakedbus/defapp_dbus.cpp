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
#include "defapp_dbus.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

Defapp_DBUS::Defapp_DBUS(QObject *parent)
    : QObject(parent)
{
}

Defapp_DBUS::~Defapp_DBUS()
{
}

void Defapp_DBUS::AddUserApp(const QStringList &mimeTypes, const QString &desktopId)
{
}

void Defapp_DBUS::AddUserAppQueued(const QStringList &mimeTypes, const QString &desktopId)
{
}

void Defapp_DBUS::DeleteApp(const QStringList &mimeTypes, const QString &desktopId)
{
}

void Defapp_DBUS::DeleteAppQueued(const QStringList &mimeTypes, const QString &desktopId)
{
}

void Defapp_DBUS::DeleteUserApp(const QString &desktopId)
{
}

void Defapp_DBUS::DeleteUserAppQueued(const QString &desktopId)
{
}

QString Defapp_DBUS::GetDefaultApp(const QString &mimeType)
{
    QJsonObject obj;
    if (mimeType == "x-scheme-handler/http") {
        obj.insert("Id", "uos-browser.desktop");
        obj.insert("Name", "浏览器");
        obj.insert("DisplayName", "浏览器");
        obj.insert("Description", "访问互联网");
        obj.insert("Icon", "uos-browser");
        obj.insert("Exec", "/usr/bin/uos-browser-stable %U");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "x-scheme-handler/mailto") {
        obj.insert("Id", "deepin-mail.desktop");
        obj.insert("Name", "邮箱");
        obj.insert("DisplayName", "邮箱");
        obj.insert("Description", "Easy to use email client");
        obj.insert("Icon", "deepin-mail");
        obj.insert("Exec", "deepin-mail %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "audio/mpeg") {
        obj.insert("Id", "deepin-music.desktop");
        obj.insert("Name", "音乐");
        obj.insert("DisplayName", "音乐");
        obj.insert("Description", "为您播放本地及网络音频流");
        obj.insert("Icon", "deepin-music");
        obj.insert("Exec", "deepin-music %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "video/mp4") {
        obj.insert("Id", "deepin-movie.desktop");
        obj.insert("Name", "影院");
        obj.insert("DisplayName", "影院");
        obj.insert("Description", "为您播放本地及网络视频");
        obj.insert("Icon", "deepin-movie");
        obj.insert("Exec", "deepin-movie %U");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "image/jpeg") {
        obj.insert("Id", "deepin-album.desktop");
        obj.insert("Name", "相册");
        obj.insert("DisplayName", "相册");
        obj.insert("Description", "深度相册");
        obj.insert("Icon", "deepin-album");
        obj.insert("Exec", "deepin-album %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "application/x-terminal") {
        obj.insert("Id", "deepin-terminal.desktop");
        obj.insert("Name", "终端");
        obj.insert("DisplayName", "终端");
        obj.insert("Description", "使用命令行");
        obj.insert("Icon", "deepin-terminal");
        obj.insert("Exec", "deepin-terminal");
        obj.insert("CanDelete", "false");
    }

    return QJsonDocument(obj).toJson();
}

QString Defapp_DBUS::ListApps(const QString &mimeType)
{
    QJsonObject obj;
    if (mimeType == "x-scheme-handler/http") {
        obj.insert("Id", "uos-browser.desktop");
        obj.insert("Name", "浏览器");
        obj.insert("DisplayName", "浏览器");
        obj.insert("Description", "访问互联网");
        obj.insert("Icon", "uos-browser");
        obj.insert("Exec", "/usr/bin/uos-browser-stable %U");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "x-scheme-handler/mailto") {
        obj.insert("Id", "deepin-mail.desktop");
        obj.insert("Name", "邮箱");
        obj.insert("DisplayName", "邮箱");
        obj.insert("Description", "Easy to use email client");
        obj.insert("Icon", "deepin-mail");
        obj.insert("Exec", "deepin-mail %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "audio/mpeg") {
        obj.insert("Id", "deepin-music.desktop");
        obj.insert("Name", "音乐");
        obj.insert("DisplayName", "音乐");
        obj.insert("Description", "为您播放本地及网络音频流");
        obj.insert("Icon", "deepin-music");
        obj.insert("Exec", "deepin-music %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "video/mp4") {
        obj.insert("Id", "deepin-movie.desktop");
        obj.insert("Name", "影院");
        obj.insert("DisplayName", "影院");
        obj.insert("Description", "为您播放本地及网络视频");
        obj.insert("Icon", "deepin-movie");
        obj.insert("Exec", "deepin-movie %U");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "image/jpeg") {
        obj.insert("Id", "deepin-album.desktop");
        obj.insert("Name", "相册");
        obj.insert("DisplayName", "相册");
        obj.insert("Description", "深度相册");
        obj.insert("Icon", "deepin-album");
        obj.insert("Exec", "deepin-album %F");
        obj.insert("CanDelete", "false");
    } else if (mimeType == "application/x-terminal") {
        obj.insert("Id", "deepin-terminal.desktop");
        obj.insert("Name", "终端");
        obj.insert("DisplayName", "终端");
        obj.insert("Description", "使用命令行");
        obj.insert("Icon", "deepin-terminal");
        obj.insert("Exec", "deepin-terminal");
        obj.insert("CanDelete", "false");
    }

    QJsonArray array;
    array.append(obj);
    return QJsonDocument(array).toJson();
}

QString Defapp_DBUS::ListUserApps(const QString &mimeType)
{
    return QString();
}

void Defapp_DBUS::SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId)
{
}

void Defapp_DBUS::SetDefaultAppQueued(const QStringList &mimeTypes, const QString &desktopId)
{
}

