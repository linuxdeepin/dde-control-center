/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NORMALLABEL_H
#define NORMALLABEL_H

#include <QLabel>

class NormalLabel : public QLabel
{
    Q_OBJECT
public:
    explicit NormalLabel(QWidget *parent = 0);
    NormalLabel(QString title, QWidget *parent = 0);
};

#endif // NORMALLABEL_H
