/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "clock.h"

#include <DSvgRenderer>
#include <DStyle>

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QtMath>
#include <QStyle>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;
DWIDGET_USE_NAMESPACE

Clock::Clock(QWidget *parent)
    : QWidget(parent)
    , m_drawTicks(true)
    , m_autoNightMode(true)
    , n_bIsUseBlackPlat(true)
    , m_render(new DSvgRenderer)
{

}

Clock::~Clock()
{

}

void Clock::setPath(const QString &picPath)
{
    m_render->load(picPath);
    QSize defaultSize = m_render->defaultSize();

    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;
    setFixedSize(defaultSize.width() + 2 * totalSpace, defaultSize.height() + 2 * totalSpace);
    update();
}

void Clock::paintEvent(QPaintEvent *)
{
    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;

    QDateTime datetime(QDateTime::currentDateTimeUtc());
    datetime = datetime.addSecs(m_timeZone.getUTCOffset());

    const QTime time(datetime.time());

    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

    // draw plate
    painter.translate(0, 0);
    if (n_bIsUseBlackPlat) {
        setPath(":/datetime/icons/dcc_clock_black.svg");
    } else {
        setPath(":/datetime/icons/dcc_clock_white.svg");
    }

    const auto ratio = devicePixelRatioF();
    QSize defaultSize = m_render->defaultSize() * ratio;
    QImage img = m_render->toImage(defaultSize);
    QRect picRect = rect().adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    m_render->render(&painter, picRect);
    painter.drawImage(picRect, img, img.rect());

    QPixmap pix;

    int nHour = (time.hour() >= 12) ? (time.hour() - 12) : time.hour();
    int nStartAngle = 90;//The image from 0 start , but the clock need from -90 start

    // draw hour hand
    const qreal hourAngle = qreal(nHour * 30 + time.minute() * 30 / 60 + time.second() * 30 / 60 / 60 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(hourAngle);
    pix.load(":/datetime/icons/dcc_noun_hour.svg");
    painter.drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
    painter.restore();

    // draw minute hand
    const qreal minuteAngle = qreal(time.minute() * 6 + time.second() * 6 / 60 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(minuteAngle);
    pix.load(":/datetime/icons/dcc_noun_minute.svg");
    painter.drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
    painter.restore();

    // draw second hand
    const qreal secondAngle = qreal(time.second() * 6 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(secondAngle);
    pix.load(":/datetime/icons/dcc_noun_second.svg");
    painter.drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
    painter.restore();

    painter.end();
}

bool Clock::autoNightMode() const
{
    return m_autoNightMode;
}

void Clock::setAutoNightMode(bool autoNightMode)
{
    if (m_autoNightMode != autoNightMode) {
        m_autoNightMode = autoNightMode;
        update();
    }
}

void Clock::setPlate(bool isBlack)
{
    if (n_bIsUseBlackPlat != isBlack) {
        n_bIsUseBlackPlat = isBlack;
        update();
    }
}

void Clock::setTimeZone(const ZoneInfo &timeZone)
{
    if (m_timeZone == timeZone) return;

    m_timeZone = timeZone;
    update();
}

bool Clock::drawTicks() const
{
    return m_drawTicks;
}

void Clock::setDrawTicks(bool drawTicks)
{
    if (m_drawTicks != drawTicks) {
        m_drawTicks = drawTicks;
        update();
    }
}
