/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <QFrame>

namespace dcc {
namespace widgets {

class HSeparator : public QFrame
{
    Q_OBJECT
public:
    explicit HSeparator(QWidget *parent = 0);
};

class VSeparator : public QFrame
{
    Q_OBJECT
public:
    explicit VSeparator(QWidget *parent = 0);
};

} // namespace widgets
} // namespace dcc

#endif // SEPARATOR_H
