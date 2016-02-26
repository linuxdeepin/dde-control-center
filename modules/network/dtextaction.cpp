/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <libdui/dthememanager.h>

#include "dtextaction.h"

DUI_USE_NAMESPACE

DTextAction::DTextAction(QWidget *parent) :
    DLinkButton("", parent)
{
    D_THEME_INIT_WIDGET(DTextAction);
}
