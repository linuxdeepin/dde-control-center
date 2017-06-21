#include "dccslider.h"

#include <QWheelEvent>

namespace dcc {
namespace widgets {

DCCSlider::DCCSlider(DCCSlider::SliderType type, QWidget *parent) :
    QSlider(parent)
{
    setType(type);
    setTracking(false);

    setFixedHeight(35);
}

void DCCSlider::setType(DCCSlider::SliderType type)
{
    switch(type) {
    case Vernier: setProperty("handleType", "Vernier"); break;
    case Progress: setProperty("handleType", "None"); break;
    default: setProperty("handleType", "Normal"); break;
    }
}

void DCCSlider::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

}
}
