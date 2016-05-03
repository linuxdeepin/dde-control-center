/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef IMAGENAMEBUTTON_H
#define IMAGENAMEBUTTON_H

#include <dimagebutton.h>
#include <dwidget_global.h>

DWIDGET_USE_NAMESPACE

class ImageNameButton : public DImageButton
{
public:
    explicit ImageNameButton(const QString &iconName, QWidget *parent = 0);
};

#endif // IMAGENAMEBUTTON_H
