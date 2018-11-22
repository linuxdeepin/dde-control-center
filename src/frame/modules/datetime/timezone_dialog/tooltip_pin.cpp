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

void TooltipPin::popup(const QPoint& point) {
  this->move(point.x() - this->width() / 2, point.y() - this->height());
  this->show();
}

void TooltipPin::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing,
                         true);
  const int kWidth = this->width();
  const int kHalfWidth = kWidth / 2;
  const int kHeight = this->height();

  // First draw background with round corner.
  QPainterPath background_path;
  background_path.moveTo(kWidth, kBorderRadius);
  background_path.arcTo(kWidth - kBorderDiameter, 0,
                        kBorderDiameter, kBorderDiameter,
                        0.0, 90.0);
  background_path.lineTo(kBorderRadius, 0);
  background_path.arcTo(0, 0, kBorderDiameter, kBorderDiameter, 90.0, 90.0);
  background_path.lineTo(0, kHeight - kBorderRadius - kTriangleHeight);
  background_path.arcTo(0, kHeight - kBorderDiameter - kTriangleHeight,
                        kBorderDiameter, kBorderDiameter,
                        180.0, 90.0);
  background_path.lineTo(kHalfWidth - kTriangleHeight,
                         kHeight - kTriangleHeight);
  // Draw isosceles right-angled triangle in bottom center of label.
  background_path.lineTo(kHalfWidth, kHeight);
  background_path.lineTo(kHalfWidth + kTriangleHeight,
                         kHeight - kTriangleHeight);
  background_path.lineTo(kWidth - kBorderRadius, kHeight - kTriangleHeight);
  background_path.arcTo(kWidth - kBorderDiameter,
                        kHeight - kBorderDiameter - kTriangleHeight,
                        kBorderDiameter, kBorderDiameter,
                        270.0, 90.0);
  background_path.closeSubpath();
  painter.fillPath(background_path, QBrush(Qt::white));

  // Then draw text.
  QFont label_font;
  label_font.setPixelSize(12);
  const QFontMetrics label_font_metrics(label_font);
  const int label_length = label_font_metrics.width(this->text());
  painter.setPen(QPen(Qt::black));
  painter.setFont(label_font);
  // Set text alignment to center
  painter.drawText((kWidth - label_length) / 2, 16, this->text());
}

}  // namespace installer
