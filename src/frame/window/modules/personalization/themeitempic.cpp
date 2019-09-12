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
#include "themeitempic.h"
#include <QMouseEvent>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QDebug>

#include <DStyle>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
DWIDGET_USE_NAMESPACE

ThemeItemPic::ThemeItemPic(QWidget *parent)
    : QWidget(parent)
    , m_isSelected(false)
{
}

bool ThemeItemPic::isSelected()
{
    return m_isSelected;
}

void ThemeItemPic::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

void ThemeItemPic::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing;
    setFixedSize(pixmap.width() + 2 * totalSpace, pixmap.height() + 2 * totalSpace); //extra space from picture to rect edge : 20
    update();
}

void ThemeItemPic::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        if (m_isSelected) return;
        Q_EMIT clicked();
    }
}

void ThemeItemPic::paintEvent(QPaintEvent *event)
{
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    int radius = 8;
    if (m_isSelected) {
        //draw blue rectangle
        QPen pen;
        pen.setBrush(palette().highlight());
        pen.setWidth(borderWidth);  //pen width
        painter.setPen(pen);
        QRect r = rect();
        painter.drawRoundedRect(r, radius, radius);
    }

    QPainterPath path;
    QRect r = rect().adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    path.addRoundedRect(r, radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(r, m_pixmap);
}
