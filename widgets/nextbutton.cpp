/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "nextbutton.h"

namespace dcc {
namespace widgets {

NextButton::NextButton(QWidget *parent) :
    TranslucentFrame(parent)
{
    setFixedSize(24, 24);
    DImageButton * btn = new DImageButton(this);

    connect(btn, &DImageButton::clicked, this, &NextButton::clicked);
}

} // namespace widgets
} // namespace dcc
