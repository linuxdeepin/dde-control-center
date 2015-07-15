#include <QPainter>
#include <QPainterPath>
#include <QDebug>

#include "clockpixmap.h"

static const int PanelSize  = 48;
static const int BigNumberHeight = 18;
static const int SmallNumberHeight = 9;
static const int BigNumberWidth = 8;
static const int SmallNumberWidth = 5;
static const int BigNumberSpacing = 1;
static const int SmallNumberSpacing = 1;
static const int amPmWidth = 10;
static const int amPmHeight = 5;

ClockPixmap::ClockPixmap() :
    QPixmap(PanelSize, PanelSize),
    m_time(QTime::currentTime()),
    m_in24hour(false),
    m_analog(false)
{
    // NOTE: it's essential to call fill before drawing on a QPixmap.
    fill(Qt::transparent);
}

ClockPixmap::ClockPixmap(QTime time) :
    QPixmap(PanelSize, PanelSize),
    m_time(time),
    m_in24hour(false),
    m_analog(false)
{
    fill(Qt::transparent);

    setTime(time);

    this->paint();
}

ClockPixmap::~ClockPixmap()
{

}
QTime ClockPixmap::getTime() const
{
    return m_time;
}

void ClockPixmap::setTime(const QTime &time)
{
    if (time.hour() != m_time.hour() || time.minute() != m_time.minute()) {
        m_time = time;

        this->paint();
    }
}

bool ClockPixmap::getIn24hour() const
{
    return m_in24hour;
}

void ClockPixmap::setIn24hour(bool in24hour)
{
    if (in24hour != m_in24hour) {
        m_in24hour = in24hour;

        this->paint();
    }
}
bool ClockPixmap::getAnalog() const
{
    return m_analog;
}

void ClockPixmap::setAnalog(bool analog)
{
    if (analog != m_analog) {
        m_analog = analog;

        this->paint();
    }
}


QPixmap ClockPixmap::getNamedPixmap(QString name)
{
    // TODO: use the QPixmapCache system.
    return QPixmap(QString(":/images/%1.png").arg(name));
}

QPixmap ClockPixmap::getNumberPixmap(int number, bool big)
{
    QString sizeText = big ? "big" : "small";
    QString numberText = QString::number(number);
    QString name = QString("%1%2").arg(sizeText).arg(numberText);

    return getNamedPixmap(name);
}

QList<int> ClockPixmap::getTimeTuple()
{
    QList<int> tuple;

    int hour = m_time.hour() > 12 ? m_time.hour() - 12 : m_time.hour();
    int minute = m_time.minute();

    int hourHigh = hour / 10;
    int hourLow = hour % 10;
    int minuteHigh = minute / 10;
    int minuteLow = minute % 10;

    tuple << hourHigh << hourLow << minuteHigh << minuteLow;

    return tuple;
}

void ClockPixmap::paint()
{
    QPainter painter;
    painter.begin(this);

    painter.fillRect(rect(), Qt::transparent);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // draw background
    QPixmap background = getNamedPixmap("panel");
    painter.drawPixmap(rect(), background);

    if (m_analog) {
        // draw analog clock
        int hourRotation = (m_time.hour() % 12) * 360 / 12;
        int minuteRotation = m_time.minute() * 360 / 60;

        // draw dial
        QPixmap dial = getNamedPixmap("dail");
        painter.drawPixmap((rect().width() - dial.width()) / 2,
                           (rect().height() - dial.height()) / 2,
                           dial);

        QPixmap hourHand = getNamedPixmap("hour_hand_with_shadow");
        QPixmap minuteHand = getNamedPixmap("minute_hand_with_shadow");

        int halfWidth = rect().width() / 2;
        int halfHeight = rect().height() / 2;

        // draw hour hand
        painter.save();

        painter.translate(halfWidth, halfHeight);
        painter.rotate(hourRotation);
        painter.drawPixmap(-halfWidth, -halfHeight, hourHand);

        painter.restore();

        // draw minute hand
        painter.save();

        painter.translate(halfWidth, halfHeight);
        painter.rotate(minuteRotation);
        painter.drawPixmap(-halfWidth, -halfHeight, minuteHand);

        painter.restore();

        // draw central point
        QPixmap point = getNamedPixmap("point");

        painter.drawPixmap((rect().width() - point.width()) / 2,
                           (rect().height() - point.height()) / 2,
                           point);

    } else {
        // draw text time
        QList<int> tuple = getTimeTuple();

        // draw hours
        int centerBaseLine = PanelSize * 11 / 20;
        painter.drawPixmap(centerBaseLine - BigNumberWidth * 2 - BigNumberSpacing,
                           (PanelSize - BigNumberHeight) / 2,
                           getNumberPixmap(tuple.at(0)));
        painter.drawPixmap(centerBaseLine - BigNumberWidth,
                           (PanelSize - BigNumberHeight) / 2,
                           getNumberPixmap(tuple.at(1)));
        // draw minutes
        if (m_in24hour) {
            painter.drawPixmap(centerBaseLine + SmallNumberSpacing,
                               PanelSize / 2 + BigNumberHeight / 2 - SmallNumberHeight,
                               getNumberPixmap(tuple.at(2), false));
            painter.drawPixmap(centerBaseLine + SmallNumberWidth + SmallNumberSpacing * 2,
                               PanelSize / 2 + BigNumberHeight / 2 - SmallNumberHeight,
                               getNumberPixmap(tuple.at(3), false));
        } else {
            painter.drawPixmap(centerBaseLine + SmallNumberSpacing,
                               PanelSize / 2 - BigNumberHeight / 2 + 2,
                               getNumberPixmap(tuple.at(2), false));
            painter.drawPixmap(centerBaseLine + SmallNumberWidth + SmallNumberSpacing * 2,
                               PanelSize / 2 - BigNumberHeight / 2 + 2,
                               getNumberPixmap(tuple.at(3), false));

            // draw am/pm mark
            QString mark = m_time.hour() > 12 ? "pm" : "am";
            painter.drawPixmap(centerBaseLine + 2,
                               PanelSize / 2 + BigNumberHeight / 2 - amPmHeight,
                               getNamedPixmap(mark));
        }
    }

    painter.end();
}
