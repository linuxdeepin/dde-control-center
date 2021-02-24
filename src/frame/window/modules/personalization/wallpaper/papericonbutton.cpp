/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#include "papericonbutton.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::personalization;

PaperIconButton::PaperIconButton(QWidget *parent)
    : DWidget(parent)
    , m_opacity(255)
    , m_radius(0)
    , m_hover(false)
{
}

void PaperIconButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QColor color;
    color = palette().color(QPalette::Base);
    color.setAlpha(m_hover ? m_opacity : m_opacity / 2);

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect(), m_radius, m_radius);

    if (!m_icon.isNull()) {
        painter.drawPixmap(rect(), m_icon);
    }

    DWidget::paintEvent(event);
}

void PaperIconButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos())) {
        Q_EMIT clicked();
    }

    DWidget::mouseReleaseEvent(event);
}

void PaperIconButton::enterEvent(QEvent *event)
{
    m_hover = true;
    update();
    DWidget::enterEvent(event);
}

void PaperIconButton::leaveEvent(QEvent *event)
{
    m_hover = false;
    update();
    DWidget::leaveEvent(event);
}

void PaperIconButton::setIcon(const QPixmap &pix)
{
    m_icon = pix;
    update();
}

void PaperIconButton::setOpacity(const int opacity)
{
    if (opacity < 0 || opacity > 255) {
        qWarning() << "set invalid opacity" << opacity;
        return;
    }

    m_opacity = opacity;
    update();
}

void PaperIconButton::setRadius(const int radius)
{
    m_radius = radius;
    update();
}
