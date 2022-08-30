// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/nextbutton.h"

#include <QVBoxLayout>

namespace dcc {
namespace widgets {

NextButton::NextButton(QWidget *parent) :
    TranslucentFrame(parent)
{
    DIconButton *btn = new DIconButton(this);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(btn);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(btn, &DIconButton::clicked, this, &NextButton::clicked);
}

} // namespace widgets
} // namespace dcc
