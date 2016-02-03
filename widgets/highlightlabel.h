/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef HIGHLIGHTLABEL_H
#define HIGHLIGHTLABEL_H

#include <QLabel>

class HighlightLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HighlightLabel(QWidget *parent = 0);
    HighlightLabel(QString text, QWidget *parent = 0);
};

#endif // HIGHLIGHTLABEL_H
