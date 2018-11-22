/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QtMath>

namespace dcc {
namespace datetime {

Clock::Clock(QWidget *parent) :
    QWidget(parent),
    m_drawTicks(true),
    m_autoNightMode(true)
{

}

Clock::~Clock()
{

}

void Clock::paintEvent(QPaintEvent *)
{
    QDateTime datetime( QDateTime::currentDateTimeUtc() );
    datetime = datetime.addSecs(m_timeZone.getUTCOffset());

    const QTime time(datetime.time());
    const QRect rct(rect());
    const int rWidth = 30;
    const int rHeight = 30;

    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    // draw plate
    const bool nightMode = !(6 < time.hour() && time.hour() < 18);
    painter.setPen(Qt::transparent);
    painter.setBrush(nightMode && autoNightMode() ? Qt::black : Qt::white);
    painter.drawRoundedRect(rct, rct.width() / 2.0, rct.height() / 2.0);

    // draw ticks
    if (m_drawTicks) {
        QStringList ticks;
        ticks << "1" << "2" << "3" << "4" << "5" << "6";
        ticks << "7" << "8" << "9" << "10" << "11" << "12";

        QPainterPath path(QPointF(rct.width() / 2.0, 0));
        for (int i = 1; i <= ticks.length(); i++) {
            QRect innerRect( rct.adjusted(rWidth / 2, rHeight / 2, - rWidth / 2, - rHeight / 2) );

            path.arcMoveTo(innerRect, 90 - 30 * i);
            QPointF pos = path.currentPosition();
            QRect r(pos.x() - rWidth / 2, pos.y() - rHeight / 2, rWidth, rHeight);

            painter.setPen(Qt::black);
            painter.drawText(r, Qt::AlignCenter, ticks.at(i - 1));
        }
    }

    QPen pen( painter.pen() );

    // draw hour hand
    const qreal hourAngle = qreal(time.hour()) * 30 + time.minute() * 30 / 60;
    painter.save();
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);
    painter.translate(rct.width() / 2.0, rct.height() / 2.0);
    painter.rotate(hourAngle);
    pen.setColor(QColor("#07c5fb"));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawLine(QPointF(0, 0), QPointF(0, -rct.width() / 2 * 0.5));
    painter.restore();

    // draw minute hand
    const int minuteAngle = time.minute() * 6;
    painter.save();
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);
    painter.translate(rct.width() / 2.0, rct.height() / 2.0);
    painter.rotate(minuteAngle);
    pen.setColor(QColor("#f97676"));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(QPointF(0, 0), QPointF(0, -rct.width() / 2 * 0.65));
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

}
}
