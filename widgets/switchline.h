/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SWITCHLINE_H
#define SWITCHLINE_H

#include <QWidget>
#include "libdui/dseparatorhorizontal.h"
#include "libdui/dswitchbutton.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class SwitchLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit SwitchLine(QWidget *parent = 0);
    bool check() const;
    void setCheck(bool s);

signals:
    void checkChanged(bool s);

private:
    void setContent(QWidget *content);

private:
    DSwitchButton *m_button = NULL;
};

#endif // SWITCHLINE_H
