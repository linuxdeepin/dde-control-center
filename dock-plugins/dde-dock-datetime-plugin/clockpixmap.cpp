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

}

ClockPixmap::ClockPixmap(QTime time) :
    QPixmap(PanelSize, PanelSize),
    m_time(time),
    m_in24hour(false),
    m_analog(false)
{
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
    painter.setRenderHint(QPainter::Antialiasing);

    // draw background
    QPixmap background = getNamedPixmap("panel");
    painter.drawPixmap(rect(), background);

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
        painter.drawPixmap(centerBaseLine + 2,
                           PanelSize / 2 + BigNumberHeight / 2 - amPmHeight,
                           getNamedPixmap("am"));
    }

    painter.end();
}
