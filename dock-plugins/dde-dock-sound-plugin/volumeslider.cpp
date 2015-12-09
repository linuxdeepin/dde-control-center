#include "volumeslider.h"
#include <QMouseEvent>

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


void VolumeSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (orientation() == Qt::Vertical) {
            setValue(minimum() + ((maximum() - minimum()) * (height() - event->y())) / height()) ;
        } else {
            setValue(minimum() + ((maximum() - minimum()) * event->x() / width())) ;
        }

        event->accept();
    }

    QSlider::mousePressEvent(event);
}

