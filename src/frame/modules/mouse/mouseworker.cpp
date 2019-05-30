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
using namespace dcc;
using namespace dcc::mouse;
const QString Service = "com.deepin.daemon.InputDevices";

MouseWorker::MouseWorker(MouseModel *model, QObject *parent)
    : QObject(parent)
    , m_dbusMouse(new Mouse(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this))
    , m_dbusTouchPad(new TouchPad(Service, "/com/deepin/daemon/InputDevice/TouchPad", QDBusConnection::sessionBus(), this))
    , m_dbusTrackPoint(new TrackPoint(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this))
    , m_dbusDevices(new InputDevices(Service, "/com/deepin/daemon/InputDevices", QDBusConnection::sessionBus(), this))
    , m_model(model)
{
    connect(m_dbusMouse, &Mouse::ExistChanged, m_model, &MouseModel::setMouseExist);
    connect(m_dbusMouse, &Mouse::LeftHandedChanged, this, &MouseWorker::setLeftHandState);
    connect(m_dbusMouse, &Mouse::NaturalScrollChanged, this, &MouseWorker::setMouseNaturalScrollState);
    connect(m_dbusMouse, &Mouse::DoubleClickChanged, this, &MouseWorker::setDouClick);
    connect(m_dbusMouse, &Mouse::DisableTpadChanged, this, &MouseWorker::setDisTouchPad);
    connect(m_dbusMouse, &Mouse::AdaptiveAccelProfileChanged, this, &MouseWorker::setAccelProfile);
    connect(m_dbusMouse, &Mouse::MotionAccelerationChanged, this, &MouseWorker::setMouseMotionAcceleration);

    connect(m_dbusTouchPad, &TouchPad::ExistChanged, m_model, &MouseModel::setTpadExist);
    connect(m_dbusTouchPad, &TouchPad::NaturalScrollChanged, this, &MouseWorker::setTouchNaturalScrollState);
    connect(m_dbusTouchPad, &TouchPad::DisableIfTypingChanged, this, &MouseWorker::setDisTyping);
    connect(m_dbusTouchPad, &TouchPad::DoubleClickChanged, this, &MouseWorker::setDouClick);
    connect(m_dbusTouchPad, &TouchPad::MotionAccelerationChanged, this, &MouseWorker::setTouchpadMotionAcceleration);
    connect(m_dbusTouchPad, &TouchPad::TapClickChanged, this, &MouseWorker::setTapClick);
    connect(m_dbusTouchPad, &TouchPad::PalmDetectChanged, m_model, &MouseModel::setPalmDetect);
    connect(m_dbusTouchPad, &TouchPad::PalmMinWidthChanged, m_model, &MouseModel::setPalmMinWidth);
    connect(m_dbusTouchPad, &TouchPad::PalmMinZChanged, m_model, &MouseModel::setPalmMinz);

    connect(m_dbusTrackPoint, &TrackPoint::ExistChanged, m_model, &MouseModel::setRedPointExist);
    connect(m_dbusTrackPoint, &TrackPoint::MotionAccelerationChanged, this, &MouseWorker::setTrackPointMotionAcceleration);

    connect(m_dbusDevices, &InputDevices::WheelSpeedChanged, m_model, &MouseModel::setScrollSpeed);

    m_dbusMouse->setSync(false);
    m_dbusTouchPad->setSync(false);
    m_dbusTouchPad->setSync(false);
    m_dbusDevices->setSync(false);
}

void MouseWorker::active()
{
    m_dbusMouse->blockSignals(false);
    m_dbusTouchPad->blockSignals(false);
    m_dbusTrackPoint->blockSignals(false);

    init();
}

void MouseWorker::deactive()
{
    m_dbusMouse->blockSignals(true);
    m_dbusTouchPad->blockSignals(true);
    m_dbusTrackPoint->blockSignals(true);
}

void MouseWorker::init()
{
    setLeftHandState(m_dbusMouse->leftHanded());
    setMouseNaturalScrollState(m_dbusMouse->naturalScroll());
    setTouchNaturalScrollState(m_dbusTouchPad->naturalScroll());
    setDisTyping(m_dbusTouchPad->disableIfTyping());
    setDisTouchPad(m_dbusMouse->disableTpad());
    setTapClick(m_dbusTouchPad->tapClick());
    setDouClick(m_dbusMouse->doubleClick());
    setAccelProfile(m_dbusMouse->adaptiveAccelProfile());
    setMouseMotionAcceleration(m_dbusMouse->motionAcceleration());
    setTouchpadMotionAcceleration(m_dbusTouchPad->motionAcceleration());
    setTrackPointMotionAcceleration(m_dbusTrackPoint->motionAcceleration());

    m_model->setMouseExist(m_dbusMouse->exist());
    m_model->setTpadExist(m_dbusTouchPad->exist());
    m_model->setRedPointExist(m_dbusTrackPoint->exist());

    m_model->setPalmDetect(m_dbusTouchPad->palmDetect());
    m_model->setPalmMinWidth(m_dbusTouchPad->palmMinWidth());
    m_model->setPalmMinz(m_dbusTouchPad->palmMinZ());

    m_model->setScrollSpeed(m_dbusDevices->wheelSpeed());
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
    m_dbusTouchPad->setPalmDetect(palmDetect);
}

void MouseWorker::setPalmMinWidth(int palmMinWidth)
{
    m_dbusTouchPad->setPalmMinWidth(palmMinWidth);
}

void MouseWorker::setPalmMinz(int palmMinz)
{
    m_dbusTouchPad->setPalmMinZ(palmMinz);
}

void MouseWorker::setScrollSpeed(int speed)
{
    m_dbusDevices->setWheelSpeed(speed);
}

void MouseWorker::onDefaultReset()
{
    m_dbusMouse->Reset();
    m_dbusTouchPad->Reset();
    m_dbusTrackPoint->Reset();
}

void MouseWorker::onLeftHandStateChanged(const bool state)
{
    m_dbusMouse->setLeftHanded(state);
    m_dbusTouchPad->setLeftHanded(state);
    m_dbusTrackPoint->setLeftHanded(state);
}

void MouseWorker::onMouseNaturalScrollStateChanged(const bool state)
{
    m_dbusMouse->setNaturalScroll(state);
}

void MouseWorker::onTouchNaturalScrollStateChanged(const bool state)
{
    m_dbusTouchPad->setNaturalScroll(state);
}

void MouseWorker::onDisTypingChanged(const bool state)
{
    m_dbusTouchPad->setDisableIfTyping(state);
}

void MouseWorker::onDisTouchPadChanged(const bool state)
{
    m_dbusMouse->setDisableTpad(state);
}

void MouseWorker::onTapClick(const bool state)
{
    m_dbusTouchPad->setTapClick(state);
}

void MouseWorker::onDouClickChanged(const int &value)
{
    m_dbusMouse->setDoubleClick(converToDouble(value));
    m_dbusTouchPad->setDoubleClick(converToDouble(value));
}

void MouseWorker::onMouseMotionAccelerationChanged(const int &value)
{
    m_dbusMouse->setMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onAccelProfileChanged(const bool state)
{
    m_dbusMouse->setAdaptiveAccelProfile(state);
}

void MouseWorker::onTouchpadMotionAccelerationChanged(const int &value)
{
    m_dbusTouchPad->setMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onTrackPointMotionAccelerationChanged(const int &value)
{
    m_dbusTrackPoint->setMotionAcceleration(converToMotionAcceleration(value));
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
        return 2.8;
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
