/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dthememanager.h>

#include "normallabel.h"

DWIDGET_USE_NAMESPACE

NormalLabel::NormalLabel(QWidget *parent)
    : QLabel(parent)
{
    D_THEME_INIT_WIDGET(NormalLabel);
}

NormalLabel::NormalLabel(QString title, QWidget *parent)
    : QLabel(title, parent)
{
    D_THEME_INIT_WIDGET(NormalLabel);
}
