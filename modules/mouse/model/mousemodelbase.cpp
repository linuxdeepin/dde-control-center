#include "mousemodelbase.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModelBase::MouseModelBase(QObject *parent)
    : QObject(parent)
{

}

void MouseModelBase::setSliderValue(const int &value)
{
    if (m_sliderValue != value) {
        m_sliderValue = value;
        emit sliderValueChanged(value);
    }
}
