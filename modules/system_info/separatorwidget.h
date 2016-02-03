/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SEPARATORWIDGET_H
#define SEPARATORWIDGET_H

#include <QFrame>

class SeparatorWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SeparatorWidget(QWidget *parent = 0);
};

class HSeparatorWidget : public SeparatorWidget
{
    Q_OBJECT
public:
    explicit HSeparatorWidget(QWidget *parent = 0);
};

class VSeparatorWidget : public SeparatorWidget
{
    Q_OBJECT
public:
    explicit VSeparatorWidget(QWidget *parent = 0);
};

#endif // SEPARATORWIDGET_H
