/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "roundcolorwidget.h"

#include <DStyle>

#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
DWIDGET_USE_NAMESPACE

RoundColorWidget::RoundColorWidget(const QColor &color, QWidget *parent)
    : QWidget(parent)
    , m_isSelected(false)
    , m_color(color)
{
}

bool RoundColorWidget::isSelected()
{
    return m_isSelected;
}

void RoundColorWidget::setSelected(bool selected)
{
    if (m_isSelected == selected)
        return;

    m_isSelected = selected;

    update();
}

void RoundColorWidget::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void RoundColorWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if (m_isSelected) return;
        Q_EMIT clicked();
    }
}

void RoundColorWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    return;
}

void RoundColorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing;
    QRect squareRect = rect();
    int delta = (squareRect.width() - squareRect.height())/2;

    if (delta != 0)
        squareRect = (delta > 0) ? squareRect.adjusted(delta + EXTRA, EXTRA, -delta - EXTRA, -EXTRA)
                                 : squareRect.adjusted(EXTRA, -delta + EXTRA , -EXTRA, delta - EXTRA);

    QPainterPath path;
    QRect r = squareRect.adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    path.addEllipse(r);
    painter.setClipPath(path);
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
    painter.fillPath(path, QBrush(m_color));
}
