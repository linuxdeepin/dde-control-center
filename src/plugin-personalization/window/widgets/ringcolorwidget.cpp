//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "ringcolorwidget.h"

#include <DPaletteHelper>
#include <DStyle>

#include <QPainter>
#include <QStyle>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

RingColorWidget::RingColorWidget(QWidget *parent)
    : QWidget{ parent }
    , m_selectedItem(nullptr)
{
    setAccessibleName("RingColorWidget");
}

void RingColorWidget::paintEvent(QPaintEvent *event)
{
    const DPalette &dp = DPaletteHelper::instance()->palette(this);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(dp.brush(DPalette::ItemBackground));
    painter.drawRoundedRect(rect(), 18, 18);

    QWidget::paintEvent(event);
    if (nullptr == m_selectedItem)
        return;

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    // draw select circle
    QPen pen;
    pen.setBrush(palette().highlight());
    pen.setWidth(borderWidth); // pen width
    painter.setPen(pen);
    QRect rc = m_selectedItem->geometry();
    painter.drawEllipse(QRect(rc.center().x() - 14, rc.center().y() - 14, 30, 30));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
}
