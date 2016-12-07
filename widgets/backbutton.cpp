/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "backbutton.h"

namespace dcc {
namespace widgets {

BackButton::BackButton(QWidget *parent) :
    TranslucentFrame(parent)
{
    setFixedSize(24, 24);
    DImageButton * btn = new DImageButton(this);

    connect(btn, &DImageButton::clicked, this, &BackButton::clicked);
}

} // namespace widgets
} // namespace dcc
