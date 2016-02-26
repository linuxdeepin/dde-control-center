/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "mainitem.h"

MainItem::MainItem(QWidget *parent) : QLabel(parent)
{
    updateIcon();
}

void MainItem::onDockModeChanged(Dock::DockMode mode)
{
    m_dockMode = mode;

    switch (mode) {
    case Dock::FashionMode:
        setFixedSize(Dock::APPLET_FASHION_ITEM_WIDTH,Dock::APPLET_FASHION_ITEM_HEIGHT);
        break;
    case Dock::EfficientMode:
        setFixedSize(Dock::APPLET_EFFICIENT_ITEM_WIDTH,Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
        break;
    case Dock::ClassicMode:
        setFixedSize(Dock::APPLET_CLASSIC_ITEM_WIDTH,Dock::APPLET_CLASSIC_ITEM_HEIGHT);
        break;
    default:
        break;
    }

    updateIcon();
}

void MainItem::updateIcon()
{
    QPixmap pixmap;
    switch (m_dockMode)
    {
    case Dock::EfficientMode:
    case Dock::ClassicMode:
        pixmap = QIcon::fromTheme("drive-harddisk-usb-symbolic").pixmap(Dock::APPLET_EFFICIENT_ICON_SIZE,Dock::APPLET_EFFICIENT_ICON_SIZE);
        break;
    default:
        pixmap = QIcon::fromTheme("drive-removable-dock").pixmap(Dock::APPLET_FASHION_ICON_SIZE,Dock::APPLET_FASHION_ICON_SIZE);
    }


    setPixmap(pixmap);
}
