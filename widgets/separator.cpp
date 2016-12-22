/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "separator.h"

namespace dcc {
namespace widgets {

HSeparator::HSeparator(QWidget *parent) :
    QFrame(parent)
{
    setFixedHeight(1);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

VSeparator::VSeparator(QWidget *parent) :
    QFrame(parent)
{
    setFixedWidth(1);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

} // namespace widgets
} // namespace dcc
