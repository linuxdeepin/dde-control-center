/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "generalremovebutton.h"

GeneralRemoveButton::GeneralRemoveButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(GeneralRemoveButton);

    setCheckable(true);
    setFixedSize(24, 23);   //image's size
}

void GeneralRemoveButton::enterEvent(QEvent *)
{
    emit mouseEnter();
}

void GeneralRemoveButton::leaveEvent(QEvent *)
{
    emit mouseLeave();
}

