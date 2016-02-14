/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QFrame>
#include <QtGui/QColor>

class TriangleRectangle : public QFrame
{
    Q_OBJECT

public:
    TriangleRectangle(QWidget *parent = 0);
    ~TriangleRectangle();
    void paintEvent(QPaintEvent *e);
    QColor fillColor;
    QColor strokeColor;
    int topFromRight;
    int halfwidth;
};

#endif // WIDGET_H
