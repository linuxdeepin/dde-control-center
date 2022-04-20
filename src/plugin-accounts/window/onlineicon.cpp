/*
 * Copyright (C) 2011 ~ 2019 Uniontech Technology Co., Ltd.
 *
 * Author:     wangwei <wangwei@uniontech.com>
 *
 * Maintainer: wangwei <wangwei@uniontech.com>
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

#include "onlineicon.h"

#include <QStyle>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

DCC_USE_NAMESPACE;
OnlineIcon::OnlineIcon(QWidget *parent)
    : QWidget(parent)
    , m_shadowEffect(new QGraphicsDropShadowEffect(this))
{
    m_shadowEffect->setColor(QColor(0, 0, 0, 16));      // 阴影的颜色
    m_shadowEffect->setOffset(0, 2);
    setGraphicsEffect(m_shadowEffect);
}

void OnlineIcon::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(QBrush(QColor(103, 239, 74)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    setGraphicsEffect(m_shadowEffect);
    painter.restore();
}
