/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundicon.h"

SoundIcon::SoundIcon()
{

}

QPixmap SoundIcon::getDefaultSinkDockIcon(Dock::DockMode mode, int volume, bool isMute)
{
    if (mode == Dock::FashionMode)
    {
        QString iconName = "";
        if (isMute)
            iconName = "audio-volume-%1-muted";
        else
            iconName = "audio-volume-%1";

        if (volume < 10)
            iconName = iconName.arg("000");
        else if(volume <= 90)
        {
            int tmp = int(volume / 10) * 10;
            iconName = iconName.arg("0" + QString::number(tmp));
        }
        else
            iconName = iconName.arg("100");

        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_FASHION_ICON_SIZE,Dock::APPLET_FASHION_ICON_SIZE);
    }
    else
        return getDefaultSinkIcon(mode,volume,isMute);
}

QPixmap SoundIcon::getDefaultSinkIcon(Dock::DockMode mode, int volume, bool isMute)
{
    QString iconName = "";
    if (isMute)
        iconName = "audio-volume-muted-symbolic";
    else
    {
        if (volume == 0)
            iconName = "audio-volume-muted-symbolic";
        else if (volume < 33)
            iconName = "audio-volume-low-symbolic";
        else if (volume < 66)
            iconName = "audio-volume-medium-symbolic";
        else
            iconName = "audio-volume-high-symbolic";
    }

    switch (mode)
    {
    case Dock::EfficientMode:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_EFFICIENT_ICON_SIZE,Dock::APPLET_EFFICIENT_ICON_SIZE);
    case Dock::ClassicMode:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_CLASSIC_ICON_SIZE,Dock::APPLET_CLASSIC_ICON_SIZE);
    default:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_EFFICIENT_ICON_SIZE,Dock::APPLET_EFFICIENT_ICON_SIZE);
    }
}

QPixmap SoundIcon::getDefaultSinkIcon(int size, int volume, bool isMute)
{
    QString iconName = "";
    if (isMute)
        iconName = "audio-volume-muted-symbolic";
    else
    {
        if (volume == 0)
            iconName = "audio-volume-muted-symbolic";
        else if (volume < 33)
            iconName = "audio-volume-low-symbolic";
        else if (volume < 66)
            iconName = "audio-volume-medium-symbolic";
        else
            iconName = "audio-volume-high-symbolic";
    }

    return QIcon::fromTheme(iconName).pixmap(size,size);
}

QPixmap SoundIcon::getAppSinkIcon(Dock::DockMode mode, const QString &iconName)
{
    switch (mode)
    {
    case Dock::EfficientMode:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_EFFICIENT_ICON_SIZE,Dock::APPLET_EFFICIENT_ICON_SIZE);
    case Dock::ClassicMode:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_CLASSIC_ICON_SIZE,Dock::APPLET_CLASSIC_ICON_SIZE);
    default:
        return QIcon::fromTheme(iconName).pixmap(Dock::APPLET_EFFICIENT_ICON_SIZE,Dock::APPLET_EFFICIENT_ICON_SIZE);
    }
}

QPixmap SoundIcon::getAppSinkIcon(int size, const QString &iconName)
{
    return QIcon::fromTheme(iconName).pixmap(size,size);
}
