/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GENERALREMOVEBUTTON_H
#define GENERALREMOVEBUTTON_H

#include <QWidget>
#include "dimagebutton.h"
#include "dthememanager.h"

DWIDGET_USE_NAMESPACE

class GeneralRemoveButton : public DImageButton
{
    Q_OBJECT
public:
    explicit GeneralRemoveButton(QWidget *parent = 0);

signals:
    void mouseEnter();
    void mouseLeave();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // GENERALREMOVEBUTTON_H
