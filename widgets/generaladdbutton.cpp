/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "generaladdbutton.h"

GeneralAddButton::GeneralAddButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(GeneralAddButton);
    setCheckable(false);
}

void GeneralAddButton::enterEvent(QEvent *e)
{
    DImageButton::enterEvent(e);

    emit mouseEnter();
}

void GeneralAddButton::leaveEvent(QEvent *e)
{
    DImageButton::leaveEvent(e);

    emit mouseLeave();
}

