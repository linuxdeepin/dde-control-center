#include "rotatedialog.h"
#include "displayworker.h"
#include "monitor.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>

using namespace dcc::display;

const QString Tips(QT_TR_NOOP("Left click to rotate and right click to exit"));

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent),

      m_mon(mon)
{
    connect(m_mon, &Monitor::wChanged, this, &RotateDialog::setFixedWidth);
    connect(m_mon, &Monitor::hChanged, this, &RotateDialog::setFixedHeight);
    connect(m_mon, &Monitor::xChanged, [=] (const int x) { move(x, y()); });
    connect(m_mon, &Monitor::yChanged, [=] (const int y) { move(x(), y); });

    setFixedWidth(m_mon->w());
    setFixedHeight(m_mon->h());
    move(m_mon->x(), m_mon->y());
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
    const auto rotates = m_mon->rotateList();
    const auto rotate = m_mon->rotate();

    Q_ASSERT(rotates.contains(rotate));

    const quint16 nextValue = rotates[(rotates.indexOf(rotate) + 1) % rotates.size()];

    emit requestRotate(m_mon, nextValue);
}
