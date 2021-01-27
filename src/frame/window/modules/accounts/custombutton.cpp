/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xuyanghe <xuyanghe@uniontech.com>
   *
   * Maintainer: xuyanghe <xuyanghe@uniontech.com>
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

#include "custombutton.h"

#include <QPainter>
#include <QIcon>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>

using namespace DCC_NAMESPACE::accounts;

CustomButton::CustomButton(QWidget *parent)
    : DWidget(parent)
    , m_text("")
    , m_icon("")
    , m_dueModel(false)
    , m_hover(false)
    , m_opacity(0)
    , m_radius(8)
    , m_hasBackColor(false)
{
    setMinimumWidth(100);
    setFixedHeight(34);
}

void CustomButton::setText(const QString &text)
{
    m_text = text;
    update();
}

void CustomButton::setIcon(const QString &icon)
{
    m_icon = icon;
    update();
}

void CustomButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QColor color;
    if (m_hasBackColor) {
        color = palette().color(QPalette::Button);
        color.setAlpha(m_hover ? m_opacity : (m_opacity / 2));
    } else {
        color = palette().color(QPalette::Button);
        color.setAlpha(m_hover ? 255 : 200);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect(), m_radius, m_radius);
    QTextOption option;
    option.setAlignment(m_dueModel ? (Qt::AlignLeft | Qt::AlignVCenter) : Qt::AlignCenter);
    painter.setPen(palette().color(QPalette::WindowText));
    QRect textRect = rect();
    textRect.setX(10);
    painter.drawText(m_dueModel ? textRect : rect(), m_text, option);
    if (!m_icon.isEmpty()) {
        QPixmap pix = QPixmap(m_icon);
        int pixX = width() - pix.width() * 2;
        int pixY = (height() - pix.height()) / 2;
        painter.drawPixmap(pixX, pixY, pix);
    }
}

void CustomButton::setDueModel(bool dueModel)
{
    if (m_dueModel == dueModel)
        return;

    m_dueModel = dueModel;
    update();
}

void CustomButton::setBackOpacity(int opacity)
{
    if (opacity < 0 || m_opacity > 255)
        return;
    m_opacity = opacity;
    m_hasBackColor = true;
    update();
}

void CustomButton::setRadius(int radius)
{
    m_radius = radius;
    update();
}

void CustomButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        Q_EMIT clicked();
}

void CustomButton::enterEvent(QEvent *event)
{
    m_hover = true;
    update();
    return DWidget::enterEvent(event);
}

void CustomButton::leaveEvent(QEvent *event)
{
    m_hover = false;
    update();
    return DWidget::leaveEvent(event);
}
