#include "iconlabel.h"

IconLabel::IconLabel(QWidget *parent) : QLabel(parent)
{

}

void IconLabel::mousePressEvent(QMouseEvent *)
{
    emit pressed();
}

void IconLabel::mouseReleaseEvent(QMouseEvent *)
{
    emit released();
}

void IconLabel::wheelEvent(QWheelEvent *)
{

}

