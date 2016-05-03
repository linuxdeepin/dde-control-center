/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MULTIADDCHECKBUTTON_H
#define MULTIADDCHECKBUTTON_H

#include <dimagebutton.h>
#include <dwidget_global.h>

DWIDGET_USE_NAMESPACE

class MultiAddCheckButton : public DImageButton
{
    Q_OBJECT

public:
    explicit MultiAddCheckButton(QWidget *parent = 0);
};

#endif // MULTIADDCHECKBUTTON_H
