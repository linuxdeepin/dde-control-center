/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_WIDGETS_NEXTBUTTON_H
#define DCC_WIDGETS_NEXTBUTTON_H

#include <dimagebutton.h>

#include "translucentframe.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class NextButton : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit NextButton(QWidget * parent = 0);

signals:
    void clicked();
};

} // namespace widgets
} // namespace dcc

#endif // DCC_WIDGETS_NEXTBUTTON_H
