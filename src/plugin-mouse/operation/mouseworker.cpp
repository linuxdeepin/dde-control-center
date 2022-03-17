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

#include "mouseworker.h"

DCC_USE_NAMESPACE
const QString Service = "com.deepin.daemon.InputDevices";

MouseWorker::MouseWorker(MouseModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{

}

void MouseWorker::setMouseExist(bool exist)
{
    m_model->setMouseExist(exist);
}

void MouseWorker::setTpadExist(bool exist)
{
    m_model->setTpadExist(exist);
}

void MouseWorker::setRedPointExist(bool exist)
{
    m_model->setRedPointExist(exist);
}

void MouseWorker::setLeftHandState(const bool state)
{
    m_model->setLeftHandState(state);
}

void MouseWorker::setMouseNaturalScrollState(const bool state)
{
    m_model->setMouseNaturalScroll(state);
}

void MouseWorker::setTouchNaturalScrollState(const bool state)
{
    m_model->setTpadNaturalScroll(state);
}

void MouseWorker::setDisTyping(const bool state)
{
    m_model->setDisIfTyping(state);
}

void MouseWorker::setDisTouchPad(const bool state)
{
    m_model->setDisTpad(state);
}

void MouseWorker::setTapClick(const bool state)
{
    m_model->setTapClick(state);
}

void MouseWorker::setDouClick(const int &value)
{
    m_model->setDoubleSpeed(converToDoubleModel(value));
}

void MouseWorker::setMouseMotionAcceleration(const double &value)
{
    m_model->setMouseMoveSpeed(converToModelMotionAcceleration(value));
}

void MouseWorker::setAccelProfile(const bool state)
{
    m_model->setAccelProfile(state);
}

void MouseWorker::setTouchpadMotionAcceleration(const double &value)
{
    m_model->setTpadMoveSpeed(converToModelMotionAcceleration(value));
}

void MouseWorker::setTrackPointMotionAcceleration(const double &value)
{
    m_model->setRedPointMoveSpeed(converToModelMotionAcceleration(value));
}

void MouseWorker::setPalmDetect(bool palmDetect)
{
    m_model->setPalmDetect(palmDetect);
}

void MouseWorker::setPalmMinWidth(int palmMinWidth)
{
    m_model->setPalmMinWidth(palmMinWidth);
}

void MouseWorker::setPalmMinz(int palmMinz)
{
    m_model->setPalmMinz(palmMinz);
}

void MouseWorker::setScrollSpeed(uint speed)
{
    m_model->setScrollSpeed(speed);
}


void MouseWorker::onPalmDetectChanged(bool palmDetect)
{
    Q_EMIT requestSetPalmDetect(palmDetect);
}

void MouseWorker::onPalmMinWidthChanged(int palmMinWidth)
{
    Q_EMIT requestSetPalmMinWidth(palmMinWidth);
}

void MouseWorker::onPalmMinzChanged(int palmMinz)
{
    Q_EMIT requestSetPalmMinz(palmMinz);
}

void MouseWorker::onScrollSpeedChanged(int speed)
{
    Q_EMIT requestSetScrollSpeed(static_cast<uint>(speed));
}

void MouseWorker::onLeftHandStateChanged(const bool state)
{
    Q_EMIT requestSetLeftHandState(state);
}

void MouseWorker::onMouseNaturalScrollStateChanged(const bool state)
{
    Q_EMIT requestSetMouseNaturalScrollState(state);
}

void MouseWorker::onTouchNaturalScrollStateChanged(const bool state)
{
    Q_EMIT requestSetTouchNaturalScrollState(state);
}

void MouseWorker::onDisTypingChanged(const bool state)
{
    Q_EMIT requestSetDisTyping(state);
}

void MouseWorker::onDisTouchPadChanged(const bool state)
{
    Q_EMIT requestSetDisTouchPad(state);
}

void MouseWorker::onTapClick(const bool state)
{
    Q_EMIT requestSetTapClick(state);
}

void MouseWorker::onDouClickChanged(const int &value)
{
    Q_EMIT requestSetDouClick(converToDouble(value));
}

void MouseWorker::onMouseMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetMouseMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onAccelProfileChanged(const bool state)
{
    Q_EMIT requestSetAccelProfile(state);
}

void MouseWorker::onTouchpadMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetTouchpadMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onTrackPointMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetTrackPointMotionAcceleration(converToMotionAcceleration(value));
}

int MouseWorker::converToDouble(int value)
{
    return 800 - value * 100;
}

int MouseWorker::converToDoubleModel(int value)
{
    return (800 - value) / 100;
}
//conver slider value to real value
double MouseWorker::converToMotionAcceleration(int value)
{
    switch (value) {
    case 0:
        return 3.2;
    case 1:
        return 2.3;
    case 2:
        return 1.6;
    case 3:
        return 1.0;
    case 4:
        return 0.6;
    case 5:
        return 0.3;
    case 6:
        return 0.2;
    default:
        return 1.0;
    }
}
//conver real value to slider value
int MouseWorker::converToModelMotionAcceleration(double value)
{
    if (value <= 0.2) {
        return 6;
    } else if (value <= 0.3) {
        return 5;
    } else if (value <= 0.6) {
        return 4;
    } else if (value <= 1.0) {
        return 3;
    } else if (value <= 1.6) {
        return 2;
    } else if (value <= 2.3) {
        return 1;
    } else if (value <= 3.2) {
        return 0;
    } else {
        return 3;
    }
}
