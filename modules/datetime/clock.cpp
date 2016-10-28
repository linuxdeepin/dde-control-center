#include "clock.h"

#include <QPainter>
#include <QTime>
#include <QtMath>
#include <QDebug>

const QPoint sec[4] = {QPoint(0, 10), QPoint(2, 0), QPoint(0, -75), QPoint(-2, 0)};
//const QPoint min[4] = {QPoint(0, 8), QPoint(3, 0), QPoint(0, -55), QPoint(-3, 0)};
//const QPoint hour[4] = {QPoint(0, 6), QPoint(4, 0), QPoint(0, -45), QPoint(-4, 0)};
const QPoint min[4] = {QPoint(1, -40), QPoint(-1, -40), QPoint(-1, 5),  QPoint(1, 5)};
const QPoint hour[4] = {QPoint(2, -35), QPoint(-2, -35),  QPoint(-2, 2), QPoint(2, 2)};

const qreal PI = 3.1415926;

Clock::Clock(QFrame *parent)
    :SettingsItem(parent),
      m_display(false)
{
    setFixedSize(300,300);
    m_timezone = QTimeZone::systemTimeZone();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(1000);
}

Clock::~Clock()
{
    //dtor
}

void Clock::setTimezone(const QString &timezone)
{
    m_timezone = QTimeZone(timezone.toStdString().c_str());
}

void Clock::setDisplay(bool display)
{
    m_display = display;
}

void Clock::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPainter matrix(this);
    QTime time = QDateTime::currentDateTime().toTimeZone(m_timezone).time();
    matrix.setWindow(-100, -100, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(100, 100);
    painter.setWindow(-100, -100, 200, 200);

    painter.save();
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(painter.window().adjusted(40,40,-40,-40));
    painter.restore();

    painter.save();
    for (int i = 1; i <= 12 ; ++i)
    {
        matrix.rotate(30);
        painter.setPen(QPen(Qt::black, 5));
        QPoint pos = matrix.transform().map(QPoint(0, -50));
        QRect rect(0,0,15,15);
        rect.moveCenter(pos);
        painter.drawText(rect, Qt::AlignCenter, QString().setNum(i));
    }
    painter.restore();

    static int clock = 1;
    if(clock > 12)
        clock -= 12;
    /*
    int s = QTime::currentTime().second();
    int m = QTime::currentTime().minute();
    int h = QTime::currentTime().hour();
    */
//    int s = time.second();
    int m = time.minute();
    int h = time.hour();

    if(m_display)
    {
        painter.save();
        QRect rect(-100, 50, 200,50);
        QFont font = painter.font();
        font.setPixelSize(11);
        painter.setFont(font);
        QPen pen = painter.pen();
        pen.setBrush(Qt::white);
        painter.setPen(pen);
        painter.drawText(rect,Qt::AlignCenter, QDateTime::currentDateTime().toString("yyyy年MM月dd日 dddd"));
        painter.restore();
    }

    painter.save();
    painter.rotate(((h % 12) +  static_cast<double>(m / 60.0)) * 30);
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);
    painter.setOpacity(0.5);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(hour, 4);
    painter.restore();

    painter.save();
    painter.rotate(m * 6);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    painter.setOpacity(0.5);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(min, 4);
    painter.restore();

//    painter.save();
//    painter.rotate(s * 6);
//    painter.setBrush(Qt::red);
//    painter.setPen(Qt::red);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.drawConvexPolygon(sec, 4);
//    painter.restore();
}
