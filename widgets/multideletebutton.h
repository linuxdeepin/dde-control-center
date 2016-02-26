/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MULTIDELETEBUTTON_H
#define MULTIDELETEBUTTON_H

#include <QWidget>
#include "libdui/dimagebutton.h"
#include "libdui/dthememanager.h"

DUI_USE_NAMESPACE

class MultiDeleteButton : public DImageButton
{
    Q_OBJECT
public:
    explicit MultiDeleteButton(QWidget *parent = 0);
};

#endif // MULTIDELETEBUTTON_H
