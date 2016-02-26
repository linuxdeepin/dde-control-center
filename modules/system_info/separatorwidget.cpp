/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "separatorwidget.h"

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

SeparatorWidget::SeparatorWidget(QWidget *parent)
    : QFrame(parent)
{
    D_THEME_INIT_WIDGET(SeparatorWidget);
}

HSeparatorWidget::HSeparatorWidget(QWidget *parent)
    : SeparatorWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(1);
}

VSeparatorWidget::VSeparatorWidget(QWidget *parent)
    : SeparatorWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(1);
}
