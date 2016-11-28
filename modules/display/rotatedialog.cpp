#include "rotatedialog.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>

const QString Tips(QT_TR_NOOP("Left click to rotate and right click to exit"));

RotateDialog::RotateDialog(MonitorInter *inter, QWidget *parent)
    : QDialog(parent),

      m_displayInter(inter)
{
    qDebug() << m_displayInter->width() << m_displayInter->height();

    connect(m_displayInter, &MonitorInter::WidthChanged, this, &RotateDialog::setFixedWidth);
    connect(m_displayInter, &MonitorInter::HeightChanged, this, &RotateDialog::setFixedHeight);
    connect(m_displayInter, &MonitorInter::XChanged, [=] (const int x) { move(x, y()); });
    connect(m_displayInter, &MonitorInter::YChanged, [=] (const int y) { move(x(), y); });

    setFixedWidth(m_displayInter->width());
    setFixedHeight(m_displayInter->height());
    move(m_displayInter->x(), m_displayInter->y());
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
}

void RotateDialog::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);

    switch (e->button())
    {
    case Qt::RightButton:   accept();       break;
    case Qt::LeftButton:    rotate();       break;
    default:;
    }
}

void RotateDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);

    QFontMetrics fm(font());
    const int tw = fm.width(Tips);
    const int w = width();
    const int h = height();
    const int margin = 100;

    QPainter painter(this);
    painter.fillRect(rect(), QColor(127, 127, 127, 255 * .6));

    // bottom
    painter.drawText((w - tw) / 2, h - margin, Tips);

    // left
    painter.rotate(90);
    painter.drawText((h - tw) / 2, -margin, Tips);

    // top
    painter.rotate(90);
    painter.drawText((w + tw) / -2, -margin, Tips);

    // right
    painter.rotate(90);
    painter.drawText((h + tw) / -2, w - margin, Tips);
}

void RotateDialog::rotate()
{
    qDebug() << "rotate";
}
