// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/translucentframe.h"

namespace dcc {
namespace widgets {

TranslucentFrame::TranslucentFrame(QWidget *parent) :
    QFrame(parent)
{
    setAccessibleName("TranslucentFrame");
}

}
} // namespace dcc
