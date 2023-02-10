//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "timezoneclock.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QtMath>

TimezoneClock::TimezoneClock(QWidget *parent)
    : QWidget(parent)
    , m_drawTicks(true)
    , m_autoNightMode(true)
{
}

TimezoneClock::~TimezoneClock()
{
}

void TimezoneClock::paintEvent(QPaintEvent *)
{
    QDateTime datetime(QDateTime::currentDateTimeUtc());
    datetime = datetime.addSecs(m_timeZone.getUTCOffset());

    const QTime time(datetime.time());

    // LCOV_EXCL_START
    //paintEvent函数单元测试无法命中，使用宏跳过此部分
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    // draw plate
    const bool nightMode = !(time.hour() >= 6 && time.hour() < 18);
    painter.setPen(Qt::transparent);
    painter.setBrush(nightMode && autoNightMode() ? Qt::black : Qt::white);

    QPen pen(painter.pen());
    if (nightMode && autoNightMode()) {
        pen.setColor(Qt::black);
    } else {
        pen.setColor(QColor("#E6E6E6"));
    }
    pen.setWidth(1);
    painter.setPen(pen);

    int penWidth = pen.width();
    const QRect rct(QRect(penWidth, penWidth, rect().width() - penWidth * 2, rect().height() - penWidth * 2));

    painter.drawRoundedRect(rct, rct.width() / 2.0, rct.height() / 2.0);

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
    // LCOV_EXCL_STOP
}

bool TimezoneClock::autoNightMode() const
{
    return m_autoNightMode;
}

void TimezoneClock::setAutoNightMode(bool autoNightMode)
{
    if (m_autoNightMode != autoNightMode) {
        m_autoNightMode = autoNightMode;
        update();
    }
}

void TimezoneClock::setTimeZone(const ZoneInfo &timeZone)
{
    if (m_timeZone == timeZone)
        return;

    m_timeZone = timeZone;
    update();
}

bool TimezoneClock::drawTicks() const
{
    return m_drawTicks;
}

void TimezoneClock::setDrawTicks(bool drawTicks)
{
    if (m_drawTicks != drawTicks) {
        m_drawTicks = drawTicks;
        update();
    }
}
