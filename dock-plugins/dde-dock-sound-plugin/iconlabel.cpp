/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "iconlabel.h"

IconLabel::IconLabel(QWidget *parent) : QLabel(parent)
{

}

void IconLabel::mousePressEvent(QMouseEvent *)
{
    emit pressed();
}

void IconLabel::mouseReleaseEvent(QMouseEvent *)
{
    emit released();
}

void IconLabel::wheelEvent(QWheelEvent *)
{

}

