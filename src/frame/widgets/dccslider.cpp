/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "widgets/dccslider.h"

#include <QWheelEvent>
#include <QDebug>

namespace dcc {
namespace widgets {

DCCSlider::DCCSlider(DCCSlider::SliderType type, QWidget *parent)
    : DSlider(Qt::Horizontal, parent)
{
    setType(type);
    DSlider::slider()->setTracking(false);
}

DCCSlider::DCCSlider(Qt::Orientation orientation, QWidget *parent)
    : DSlider(orientation, parent)
{
    DSlider::slider()->setTracking(false);
}

void DCCSlider::setType(DCCSlider::SliderType type)
{
    switch (type) {
    case Vernier: setProperty("handleType", "Vernier"); break;
    case Progress: setProperty("handleType", "None"); break;
    default: setProperty("handleType", "Normal"); break;
    }
}

QSlider *DCCSlider::qtSlider()
{
    return DSlider::slider();
}

void DCCSlider::setRange(int min, int max)
{
    setMinimum(min);
    setMaximum(max);
}

void DCCSlider::setTickPosition(QSlider::TickPosition tick)
{
    tickPosition = tick;
}

void DCCSlider::setTickInterval(int ti)
{
    DSlider::slider()->setTickInterval(ti);
}

void DCCSlider::setSliderPosition(int Position)
{
    DSlider::slider()->setSliderPosition(Position);
}

void DCCSlider::setAnnotations(const QStringList &annotations)
{
    switch (tickPosition) {
    case QSlider::TicksLeft:
        setLeftTicks(annotations);
        break;
    case QSlider::TicksRight:
        setRightTicks(annotations);
        break;
    default:
        break;
    }
}

void DCCSlider::setOrientation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
}


void DCCSlider::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

}
}
