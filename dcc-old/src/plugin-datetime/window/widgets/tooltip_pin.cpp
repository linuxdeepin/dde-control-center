// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "tooltip_pin.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

namespace installer {

namespace {

const int kBorderRadius = 4;

// _____
// \ | /
//  \|/
const int kTriangleHeight = 6;

} // namespace

TooltipPin::TooltipPin(QWidget *parent)
    : QLabel(parent)
    , m_arrowDirection(ArrowDirection::ArrowDown)
{
    this->setObjectName("tooltip_pin");

    this->setAlignment(Qt::AlignHCenter);

    // Add 15px margin horizontally.
    this->setStyleSheet("margin: 0 15px;");
}

void TooltipPin::setArrowDirection(TooltipPin::ArrowDirection arrowDirection)
{
    m_arrowDirection = arrowDirection;
}

void TooltipPin::popup(const QPoint &point)
{
    if (point.x() < 100) {
        this->move(point.x() + kTriangleHeight / 2, point.y() - this->height() / 8);
    } else {
        this->move(point.x() - this->width() / 2, point.y() - this->height() + 6);
    }
    this->show();
}

void TooltipPin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    const int kWidth = this->width();
    const int kHeight = this->height();

    QPainterPath background_path;
    QPainterPath arrow_path;

    if (ArrowLeft == m_arrowDirection) {
        // 矩形区域
        background_path.addRoundedRect(kTriangleHeight,
                                       0,
                                       kWidth - kTriangleHeight,
                                       kHeight - kTriangleHeight,
                                       kBorderRadius,
                                       kBorderRadius);

        // 三角箭头
        arrow_path.moveTo(kTriangleHeight, (kHeight - kTriangleHeight) / 2 - kTriangleHeight);
        arrow_path.lineTo(0, (kHeight - kTriangleHeight) / 2);
        arrow_path.lineTo(kTriangleHeight, (kHeight - kTriangleHeight) / 2 + kTriangleHeight);
    } else {
        // 矩形区域
        background_path.addRoundedRect(0,
                                       0,
                                       kWidth,
                                       kHeight - kTriangleHeight,
                                       kBorderRadius,
                                       kBorderRadius);

        // 三角箭头
        arrow_path.moveTo(kWidth / 2 - kTriangleHeight, kHeight - kTriangleHeight);
        arrow_path.lineTo(kWidth / 2, kHeight);
        arrow_path.lineTo(kWidth / 2 + kTriangleHeight, kHeight - kTriangleHeight);
    }

    background_path.addPath(arrow_path);

    // 画背景
    painter.drawPath(background_path);

    // Then draw text.
    QFont label_font;
    label_font.setPixelSize(12);
    const QFontMetrics label_font_metrics(label_font);
    const int label_length = label_font_metrics.horizontalAdvance(this->text());
    painter.setPen(QPen(Qt::black));
    painter.setFont(label_font);
    // Set text alignment to center
    painter.drawText((kWidth - label_length) / 2, 16, this->text());
}

} // namespace installer
