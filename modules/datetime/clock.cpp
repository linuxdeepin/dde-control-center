#include "clock.h"

#include <QPainter>
#include <QTime>
#include <QtMath>
#include <QDebug>
#include <QResizeEvent>

namespace dcc {
namespace datetime {

const QPoint sec[4] = {QPoint(0, 10), QPoint(2, 0), QPoint(0, -75), QPoint(-2, 0)};
//const QPoint min[4] = {QPoint(0, 8), QPoint(3, 0), QPoint(0, -55), QPoint(-3, 0)};
//const QPoint hour[4] = {QPoint(0, 6), QPoint(4, 0), QPoint(0, -45), QPoint(-4, 0)};
const QPoint min[4] = {QPoint(1, -60), QPoint(-1, -60), QPoint(-1, 5),  QPoint(1, 5)};
const QPoint hour[4] = {QPoint(2, -45), QPoint(-2, -45),  QPoint(-2, 2), QPoint(2, 2)};

const qreal PI = 3.1415926;

Clock::Clock(QFrame *parent)
    :SettingsItem(parent),
      m_display(false)
{
    m_timezone = QTimeZone::systemTimeZone();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(1000);
    setFixedHeight(260);
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
    QSize size = this->size();
    int minw = size.width() > size.height() ? size.height() : size.width();
    int offset = 0;
    if(m_display)
    {
        minw = 160;
        offset = -20;
    }
    QRect rect = QRect(0,0,minw,minw);
    rect.moveCenter(this->rect().center() + QPoint(0,offset));

    QPainter painter(this);
    QPainter matrix(this);
    painter.setViewport(rect);
    matrix.setViewport(rect);
    QTime time = QDateTime::currentDateTime().toTimeZone(m_timezone).time();
    matrix.setWindow(-100, -100, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(100, 100);
    painter.setWindow(-100, -100, 200, 200);

    if(m_display)
    {
        painter.save();
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawEllipse(painter.window());
        painter.restore();

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        for (int i = 1; i <= 12 ; ++i)
        {
            matrix.rotate(30);
            QFont font = painter.font();
            font.setPointSize(12);
            painter.setFont(font);
            QPoint pos = matrix.transform().map(QPoint(0, -80));
            QRect rect(0,0,25,25);
            rect.moveCenter(pos);
            painter.drawText(rect, Qt::AlignCenter, QString().setNum(i));
        }
        painter.restore();
    }
    else
    {
        painter.save();
        painter.setPen(Qt::white);

        if(time.hour() > 18 || time.hour() <6)
        {
            painter.setBrush(Qt::black);
            painter.setOpacity(0.5);
        }
        else
        {
            painter.setBrush(Qt::white);
        }
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawEllipse(painter.window());
        painter.restore();
    }

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

    painter.save();
    painter.rotate(((h % 12) +  static_cast<double>(m / 60.0)) * 30);
    painter.setBrush(QColor("#07c5fb"));
    painter.setPen(QColor("#07c5fb"));
    painter.setOpacity(0.5);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawConvexPolygon(hour, 4);
    painter.restore();

    painter.save();
    painter.rotate(m * 6);
    painter.setBrush(QColor("#f97676"));
    painter.setPen(QColor("#f97676"));
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

    if(m_display)
    {
        painter.save();
        painter.setViewport(this->rect());
        painter.setWindow(this->rect());
        QRect rect(0,this->height() - 17- 20,this->width(), 17);
        QFont font = painter.font();
        font.setPixelSize(11);
        painter.setFont(font);
        QPen pen = painter.pen();
        pen.setBrush(Qt::white);
        painter.setPen(pen);
        painter.drawText(rect,Qt::AlignCenter, QDateTime::currentDateTime().toString("yyyy年MM月dd日 dddd"));
        painter.restore();
    }

}

void Clock::resizeEvent(QResizeEvent *e)
{
    SettingsItem::resizeEvent(e);
}
}
}
