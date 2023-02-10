//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "themeitempic.h"

#include <DStyle>
#include <DSvgRenderer>

#include <QMouseEvent>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QDebug>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using DTK_GUI_NAMESPACE::DSvgRenderer;

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
    int totalSpace = borderWidth + borderSpacing + margins;
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
    int totalSpace = borderWidth + borderSpacing + margins;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //first draw image
    const auto ratio = devicePixelRatioF();
    QSize defaultSize = render->defaultSize() * ratio;
    QImage img = render->toImage(defaultSize);
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
