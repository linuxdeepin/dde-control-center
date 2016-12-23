#include "clock.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QtMath>
#include <QDebug>

namespace dcc {
namespace datetime {

Clock::Clock(QWidget *parent) :
    QWidget(parent),
    m_drawTicks(true),
    m_timeZone(QTimeZone::systemTimeZone())
{

}

Clock::~Clock()
{

}

void Clock::paintEvent(QPaintEvent *)
{
    QDateTime datetime( QDateTime::currentDateTimeUtc() );
    datetime = datetime.addSecs(m_timeZone.offsetFromUtc(datetime));

    const QTime time( datetime.time() );
    const QRect rct( rect() );
    const int rWidth = 20;
    const int rHeight = 20;

    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    // draw plate
    const bool nightMode = !(6 < time.hour() && time.hour() < 18);
    painter.setBrush(nightMode ? Qt::black : Qt::white);
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
    pen.setWidth(2);

    // draw hour hand
    const int hourAngle = time.hour() * 30;
    painter.save();
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);
    painter.translate(rct.width() / 2.0, rct.height() / 2.0);
    painter.rotate(hourAngle);
    pen.setColor(Qt::cyan);
    painter.setPen(pen);
    painter.drawLine(QPointF(0, 0), QPointF(0, -rct.width() / 2 * 0.5));
    painter.restore();

    // draw minute hand
    const int minuteAngle = time.minute() * 6;
    painter.save();
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);
    painter.translate(rct.width() / 2.0, rct.height() / 2.0);
    painter.rotate(minuteAngle);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(QPointF(0, 0), QPointF(0, -rct.width() / 2 * 0.7));
    painter.restore();

    painter.end();
}

QTimeZone Clock::timeZone() const
{
    return m_timeZone;
}

void Clock::setTimeZone(const QTimeZone &timeZone)
{
    if (m_timeZone != timeZone) {
        m_timeZone = timeZone;
        update();
    }
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
