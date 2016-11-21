#include "mousemodelbase.h"

MouseModelBase::MouseModelBase(QObject *parent)
{
    Q_UNUSED(parent);
}

void MouseModelBase::setSliderValue(const int &value)
{
    m_sliderValue = value;
    emit sliderValueChanged(value);
}
