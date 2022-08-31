// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_WIDGETS_NEXTBUTTON_H
#define DCC_WIDGETS_NEXTBUTTON_H

#include <DIconButton>

#include "widgets/translucentframe.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class NextButton : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit NextButton(QWidget * parent = 0);

Q_SIGNALS:
    void clicked();
};

} // namespace widgets
} // namespace dcc

#endif // DCC_WIDGETS_NEXTBUTTON_H
