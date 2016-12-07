#include "mousemodelbase.h"

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
