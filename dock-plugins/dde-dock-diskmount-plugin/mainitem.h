/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MAINITEM_H
#define MAINITEM_H

#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <dde-dock/dockconstants.h>

class MainItem : public QLabel
{
    Q_OBJECT
public:
    MainItem(QWidget *parent = 0);
    void onDockModeChanged(Dock::DockMode mode);

private:
    void updateIcon();

private:
    Dock::DockMode m_dockMode = Dock::FashionMode;
};

#endif // MAINITEM_H
