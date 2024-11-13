//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "roundcolorwidget.h"

#include <DStyle>

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

DWIDGET_USE_NAMESPACE

RoundColorWidget::RoundColorWidget(const QColor &color, QWidget *parent)
    : QWidget(parent)
    , m_isSelected(false)
    , m_color(color)
{
    setAccessibleName("RoundColorWidget");
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

void RoundColorWidget::setActiveColors(const QPair<QString, QString>& activeColors)
{
    if (m_activeColors == activeColors)
        return;

    m_activeColors = activeColors;
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
    if (m_color.isValid()) {
        painter.fillPath(path, QBrush(m_color));
    } else {
        QConicalGradient gradient(r.center(), 0);
        gradient.setColorAt(0, QColor(255, 0, 0));
        gradient.setColorAt(0.167, QColor(255, 255, 0));
        gradient.setColorAt(0.333, QColor(0, 255, 0));
        gradient.setColorAt(0.5, QColor(0, 255, 255));
        gradient.setColorAt(0.667, QColor(0, 0, 255));
        gradient.setColorAt(0.833, QColor(255, 0, 255));
        gradient.setColorAt(1, QColor(255, 0, 0));
        painter.fillPath(path, QBrush(gradient));
    }
}
