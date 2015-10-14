#include "volumeslider.h"

VolumeSlider::VolumeSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{

}

VolumeSlider::~VolumeSlider()
{

}
bool VolumeSlider::isMute() const
{
    return m_isMute;
}

void VolumeSlider::setIsMute(bool value)
{
    m_isMute = value;

    setStyleSheet(this->styleSheet());  // force a stylesheet recomputation
}


