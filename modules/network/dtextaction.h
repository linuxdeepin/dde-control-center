/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DTEXTACTION_H
#define DTEXTACTION_H

#include <dlinkbutton.h>

DWIDGET_USE_NAMESPACE

class DTextAction : public DLinkButton
{
public:
    explicit DTextAction(QWidget *parent = 0);
};

#endif // DTEXTACTION_H
