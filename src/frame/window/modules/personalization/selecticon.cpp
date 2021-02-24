/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#include "selecticon.h"

#include <DStyle>

#include <QPainter>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::personalization;

SelectIcon::SelectIcon(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(22, 22);
}

void SelectIcon::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QPainterPath path;

    QRect r = rect();
    path.addEllipse(rect());
    painter.setClipPath(path);
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
    painter.fillPath(path, QBrush(palette().highlight()));

    QPixmap pix(":/personalization/check.svg");
    // 图标中心点和圆重合
    QRect rect(r.x() + (r.width() - 16) / 2, r.y() + (r.height() - 16) / 2, 16, 16);
    painter.drawPixmap(rect, pix);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
}
