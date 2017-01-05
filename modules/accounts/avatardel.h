/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef AVATARDEL_H
#define AVATARDEL_H

#include <QWidget>
#include <QPaintEvent>
class AvatarDel : public QWidget
{
    Q_OBJECT
public:
    explicit AvatarDel(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);

signals:
    void click();

public slots:
};

#endif // AVATARDEL_H
