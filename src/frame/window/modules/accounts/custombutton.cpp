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
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>

const int ICON_SIZE = 12;

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

void CustomButton::setIcon(const QIcon &icon)
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
    painter.save();
    QTextOption option;
    option.setAlignment(m_dueModel ? (Qt::AlignLeft | Qt::AlignVCenter) : Qt::AlignCenter);
    painter.setPen(palette().color(QPalette::WindowText));
    QRect textRect = rect();
    textRect.setX(10);
    painter.drawText(m_dueModel ? textRect : rect(), m_text, option);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect(), m_radius, m_radius);
    painter.restore();
    if (!m_icon.isNull()) {
        int x = width() - ICON_SIZE * 2;
        int y = (height() - ICON_SIZE) / 2;
        m_icon.paint(&painter, x, y, ICON_SIZE, ICON_SIZE);
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
