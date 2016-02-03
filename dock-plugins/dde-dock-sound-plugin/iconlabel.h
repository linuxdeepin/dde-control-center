/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QWidget>
#include <QLabel>

class IconLabel : public QLabel
{
    Q_OBJECT
public:
    explicit IconLabel(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

signals:
    void pressed();
    void released();
    void clicked();
};

#endif // ICONLABEL_H
