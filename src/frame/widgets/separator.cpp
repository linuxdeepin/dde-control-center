// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/separator.h"

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
