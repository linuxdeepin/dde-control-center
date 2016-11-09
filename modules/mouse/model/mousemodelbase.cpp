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

void MouseModelBase::setSliderMaxValue(const int &value)
{
    m_sliderMaxValue = value;
}

void MouseModelBase::setSliderMinValue(const int &value)
{
    m_sliderMinValue = value;
}

void MouseModelBase::setSliderStep(const int &value)
{
    m_sliderStep = value;
}
