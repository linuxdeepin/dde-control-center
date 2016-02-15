/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef HEADERLINE_H
#define HEADERLINE_H

#include <dbaseline.h>

DWIDGET_USE_NAMESPACE

class HeaderLine : public DBaseLine
{
    Q_OBJECT
public:
    explicit HeaderLine(QString title, QWidget *parent = 0);
};

#endif // HEADERLINE_H
