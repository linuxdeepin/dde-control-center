// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "horizontalseparator.h"

#include <QPainter>

/**
 * @brief HorizontalSeparator::HorizontalSeparator 分割线控件,高度值初始化为2个像素
 * @param parent
 */
HorizontalSeparator::HorizontalSeparator(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(2);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize HorizontalSeparator::sizeHint() const
{
    return QSize(QWidget::sizeHint().width(), 2);
}

void HorizontalSeparator::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    QColor c = palette().color(QPalette::BrightText);
    c.setAlpha(int(0.1 * 255));

    painter.fillRect(rect(), c);
}
