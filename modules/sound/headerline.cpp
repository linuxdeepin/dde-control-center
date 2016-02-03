/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "headerline.h"
#include "dthememanager.h"
#include "highlightlabel.h"

HeaderLine::HeaderLine(QString title, QWidget *parent)
    : DBaseLine(parent)
{
    HighlightLabel * _title = new HighlightLabel(title, this);

    this->setLeftContent(_title);

    setStyleSheet(QString());
    D_THEME_INIT_WIDGET(HeaderLine);
}
