/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIPSLABEL_H
#define TIPSLABEL_H

#include "smalllabel.h"

namespace dcc {
namespace widgets {

class TipsLabel : public SmallLabel
{
    Q_OBJECT
public:
    explicit TipsLabel(QWidget *parent = 0);
    TipsLabel(const QString &text, QWidget *parent = 0);
};

} // namespace widgets
} // namespace dcc

#endif // TIPSLABEL_H
