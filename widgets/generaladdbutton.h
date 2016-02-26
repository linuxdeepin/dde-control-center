/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GENERALADDBUTTON_H
#define GENERALADDBUTTON_H

#include <QWidget>
#include <QEvent>
#include "libdui/dimagebutton.h"
#include "libdui/dthememanager.h"

DUI_USE_NAMESPACE

class GeneralAddButton : public DImageButton
{
    Q_OBJECT
public:
    explicit GeneralAddButton(QWidget *parent = 0);

signals:
    void mouseEnter();
    void mouseLeave();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // GENERALADDBUTTON_H
