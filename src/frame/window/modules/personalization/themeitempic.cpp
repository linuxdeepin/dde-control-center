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

#include <DStyle>
#include <DSvgRenderer>
#include <DApplicationHelper>

#include <QMouseEvent>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using DTK_GUI_NAMESPACE::DSvgRenderer;

const int ThemeItemPicRadius = 12;

ThemeItemPic::ThemeItemPic(QWidget *parent)
    : QWidget(parent)
    , m_isSelected(false)
    , render(new DSvgRenderer())
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

void ThemeItemPic::setPath(const QString &picPath)
{
    render->load(picPath);
    QSize defaultSize = render->defaultSize();

    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = DGuiApplicationHelper::isTabletEnvironment() ?  borderWidth : borderWidth + borderSpacing + margins;
    setFixedSize(defaultSize.width() + 2 * totalSpace, defaultSize.height() + 2 * totalSpace);
    update();
}

ThemeItemPic::~ThemeItemPic()
{
    render->deleteLater();
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
    Q_UNUSED(event)
    int radius = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameRadius), nullptr, nullptr);
    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);

    int totalSpace = DGuiApplicationHelper::isTabletEnvironment() ?  borderWidth : borderWidth + borderSpacing + margins;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //first draw image
    const auto ratio = devicePixelRatioF();
    QSize defaultSize = render->defaultSize() * ratio;
    QImage img = render->toImage(defaultSize);
    QRect picRect = rect().adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    painter.drawImage(picRect, img, img.rect());

    if (!DGuiApplicationHelper::isTabletEnvironment()) {
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
    }

    //last draw focus rectangle
    if (m_isSelected) {
        if (DGuiApplicationHelper::isTabletEnvironment()) {
            QRect border = rect().adjusted(margins, margins, -margins, -margins);
            int offset = 1;
            QRect rect = border.adjusted(offset, offset, -offset, -offset);

            QPen pen;
            pen.setWidth(2);
            // 先画内框
            pen.setColor(palette().base().color());
            painter.setPen(pen);
            painter.drawRoundedRect(rect, ThemeItemPicRadius, ThemeItemPicRadius);

            // 再画外框（活动色即焦点）
            pen.setColor(palette().highlight().color());
            painter.setPen(pen);
            painter.drawRoundedRect(border, ThemeItemPicRadius + margins, ThemeItemPicRadius + margins);
        } else {
            QStyleOption option;
            option.initFrom(this);
            style()->drawPrimitive(DStyle::PE_FrameFocusRect, &option, &painter, this);
        }
    }
}
