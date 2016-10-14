#include "clock.h"

#include <QPainter>
#include <QTime>
#include <QtMath>

const QPoint sec[4] = {QPoint(0, 10), QPoint(2, 0), QPoint(0, -75), QPoint(-2, 0)};
const QPoint min[4] = {QPoint(0, 8), QPoint(3, 0), QPoint(0, -55), QPoint(-3, 0)};
const QPoint hour[4] = {QPoint(0, 6), QPoint(4, 0), QPoint(0, -45), QPoint(-4, 0)};

const qreal PI = 3.1415926;

Clock::Clock(QWidget *parent):QWidget(parent)
{
    //ctor
    setFixedSize(300,300);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));//update()会自动产生重绘消息，调用paintEvent()
    m_timer->start(1000);
}

Clock::~Clock()
{
    //dtor
}
#include <QDebug>

void Clock::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPainter matrix(this);
    matrix.setWindow(-100, -100, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(100, 100);
    painter.setWindow(-100, -100, 200, 200);

    painter.save();
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(painter.window().adjusted(3,3,-3,-3));
    painter.restore();
    //绘制大刻度
    painter.save();
    for (int i = 1; i <= 12 ; ++i)
    {
        matrix.rotate(30);
        painter.setPen(QPen(Qt::black, 5));
        QPoint pos = matrix.transform().map(QPoint(0, -80));
        QRect rect(0,0,15,15);
        rect.moveCenter(pos);
        painter.drawText(rect, Qt::AlignCenter, QString().setNum(i));
    }
    painter.restore();

    static int clock = 1;
    if(clock > 12)
        clock -= 12;
    int s = QTime::currentTime().second();
    int m = QTime::currentTime().minute();
    int h = QTime::currentTime().hour();

    painter.drawText(-20, 50, QTime::currentTime().toString());

    painter.save();
    painter.rotate(((h % 12) +  static_cast<double>(m / 60.0)) * 30);
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(hour, 4);
    painter.restore();

    painter.save();
    painter.rotate(m * 6);
    painter.setBrush(Qt::green);
    painter.setPen(Qt::green);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(min, 4);
    painter.restore();

    painter.save();
    painter.rotate(s * 6);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(sec, 4);
    painter.restore();
}
