// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/backbutton.h"

namespace dcc {
namespace widgets {

BackButton::BackButton(QWidget *parent) :
    TranslucentFrame(parent)
{
    setFixedSize(24, 24);
    DIconButton *btn = new DIconButton(this);

    connect(btn, &DIconButton::clicked, this, &BackButton::clicked);
}

} // namespace widgets
} // namespace dcc
