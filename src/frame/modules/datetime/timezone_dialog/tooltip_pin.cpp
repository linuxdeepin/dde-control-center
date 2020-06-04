/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "tooltip_pin.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

namespace installer {

namespace {

const int kBorderRadius = 4;
const int kBorderDiameter = kBorderRadius * 2;

// _____
// \ | /
//  \|/
const int kTriangleHeight = 6;

}  // namespace

TooltipPin::TooltipPin(QWidget* parent) : QLabel(parent) {
  this->setObjectName("tooltip_pin");

  this->setAlignment(Qt::AlignHCenter);

  // Add 15px margin horizontally.
  this->setStyleSheet("margin: 0 15px;");
}

void TooltipPin::setArrowDirection(TooltipPin::ArrowDirection arrowDirection)
{
    m_arrowDirection = arrowDirection;
}

void TooltipPin::popup(const QPoint& point)
{
    if (point.x() < 100) {
        this->move(point.x() + 6, point.y() - this->height() / 8);
    } else {
        this->move(point.x() - this->width() / 2, point.y() - this->height() + 6);
    }
    this->show();
}

void TooltipPin::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    // Then draw text.
    QFont label_font;
    label_font.setPixelSize(12);
    const QFontMetrics label_font_metrics(label_font);
    const int label_length = label_font_metrics.width(this->text());

    int kWidth = this->width();
    int kHalfWidth = kWidth / 2;
    int kHeight = this->height();
    QPolygon trianglePolygon;

    // 三角形箭头方向
    QPainterPath arrowPath;
    switch (m_arrowDirection) {
    case ArrowLeft:
        trianglePolygon << QPoint(10, 0);
        trianglePolygon << QPoint(0, 8);
        trianglePolygon << QPoint(10, 18);
        painter.drawRoundedRect(QRect(QPoint(5, 0), QSize(kWidth - kTriangleHeight, kHeight / 2 + 3)), 8, 8);
        // Rect + Triangle;
        arrowPath.addPolygon(trianglePolygon);
        painter.drawPath(arrowPath);
        break;
    case ArrowDown:
        trianglePolygon << QPoint((kWidth - 10) / 2 + 10, 2.5 * kTriangleHeight + 3);
        trianglePolygon << QPoint(kHalfWidth, kHeight - 6);
        trianglePolygon << QPoint((kWidth + 10) / 2 - 10, 2.5 * kTriangleHeight + 3);
        painter.drawRoundedRect(QRect(QPoint(0, 0), QSize(kWidth, kHeight / 2 + 3)), kBorderRadius, kBorderRadius);
        // Rect + Triangle;
        arrowPath.addPolygon(trianglePolygon);
        painter.drawPath(arrowPath);
        break;
    }
    painter.setPen(QPen(Qt::black));
    painter.setFont(label_font);
    // Set text alignment to center
    painter.drawText((kWidth - label_length) / 2, 16, this->text());
}

}  // namespace installer
