/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPLYBUTTONLINE_H
#define APPLYBUTTONLINE_H

#include <QWidget>
#include <QHBoxLayout>
#include "dtextbutton.h"
#include "dheaderline.h"

DWIDGET_USE_NAMESPACE

class ApplyButtonLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit ApplyButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void apply();
};
#endif // APPLYBUTTONLINE_H
