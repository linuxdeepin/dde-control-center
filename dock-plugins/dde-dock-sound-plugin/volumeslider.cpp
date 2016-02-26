/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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

