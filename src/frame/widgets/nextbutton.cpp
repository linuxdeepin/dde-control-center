/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
