/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dthememanager.h>

#include "imagenamebutton.h"

ImageNameButton::ImageNameButton(const QString &iconName, QWidget *parent) :
    DImageButton(parent)
{
    QString icon_path = ":/";
    icon_path += DThemeManager::instance()->theme().append("/images/").append(iconName);
    setNormalPic(icon_path+"_normal.png");
    setPressPic(icon_path+"_press.png");
    setHoverPic(icon_path+"_hover.png");
    setCheckedPic(icon_path+"_checked.png");
}

