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
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

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
    setFixedSize(pixmap.width() + 20, pixmap.height() + 20); //extra space from picture to rect edge : 20
    update();
}

void ThemeItemPic::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        if (m_isSelected) return;
        Q_EMIT clicked();
        update();
    }
}

void ThemeItemPic::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    if (m_isSelected) {
        //draw blue rectangle
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(2);  //pen width
        painter.setPen(pen);
        QRect r = rect().adjusted(5, 5, -5, -5);
        painter.drawRoundedRect(r, 20, 20);  //radius : 20
    }
    QPainterPath path;
    QRect r = rect().adjusted(10, 10, -10, -10);
    path.addRoundedRect(r, 20, 20);  //radius : 20
    painter.setClipPath(path);
    painter.drawPixmap(r, m_pixmap);
}
