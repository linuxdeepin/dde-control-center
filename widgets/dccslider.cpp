#include "dccslider.h"

#include <QWheelEvent>

using namespace dcc;

DCCSlider::DCCSlider(QWidget *parent)
    : QSlider(parent)
{

}

void DCCSlider::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}
