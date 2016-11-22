#include "funcbutton.h"
FuncButton::FuncButton(QObject *parent)
{
    Q_UNUSED(parent);
}

void FuncButton::enterEvent(QEvent *event)
{
    setCursor(Qt::PointingHandCursor);
    event->accept();
}

void FuncButton::leaveEvent(QEvent *event)
{
    event->accept();
}

void FuncButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    event->accept();
}

void FuncButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (!rect().contains(event->pos()))
        return;

    event->accept();

    if (event->button() == Qt::LeftButton)
        emit clicked();
}
