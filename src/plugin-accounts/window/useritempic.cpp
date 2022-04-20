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
#include "useritempic.h"

#include <DStyle>
#include <DSvgRenderer>

#include <QMouseEvent>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QDebug>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using DTK_GUI_NAMESPACE::DSvgRenderer;

UserItemPic::UserItemPic(QWidget *parent)
    : QWidget(parent)
    , m_isSelected(false)
{
}

bool UserItemPic::isSelected()
{
    return m_isSelected;
}

void UserItemPic::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

void UserItemPic::setPath(const QString &picPath)
{
    m_picPath = picPath;

    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;
    setFixedSize(45 + 2 * totalSpace, 45 + 2 * totalSpace);
    update();
}

UserItemPic::~UserItemPic()
{
}

void UserItemPic::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}

void UserItemPic::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    int radius = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameRadius), nullptr, nullptr);
    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QImage img = QImage(m_picPath);
    QRect picRect = rect().adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    painter.drawImage(picRect, img, img.rect());

    //second draw picture rounded rect bound
    QPen pen;
    pen.setColor(palette().base().color());
    painter.setPen(pen);
    painter.drawRoundedRect(picRect, radius, radius);

    //third fill space with base brush
    QPainterPath picPath;
    picPath.addRect(picRect);
    QPainterPath roundPath;
    roundPath.addRoundedRect(picRect, radius, radius);
    QPainterPath anglePath = picPath - roundPath;
    painter.fillPath(anglePath, palette().base().color());
    painter.strokePath(picPath, palette().base().color());

    //last draw focus rectangle
    if (m_isSelected) {
        QStyleOption option;
        option.initFrom(this);
        style()->drawPrimitive(DStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}
