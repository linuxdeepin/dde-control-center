/*
 * Copyright (C) 2019 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     dongrui <dongrui@uniontech.com>
 *
 * Maintainer: dongrui <dongrui@uniontech.com>
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

#include "mousedbusproxy.h"
using namespace dcc;
using namespace dcc::mouse;
const QString Service = "com.deepin.daemon.InputDevices";

MouseDBusProxy::MouseDBusProxy(MouseWorker *worker, QObject *parent)
    : QObject(parent)
    , m_dbusMouse(new Mouse(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this))
    , m_dbusTouchPad(new TouchPad(Service, "/com/deepin/daemon/InputDevice/TouchPad", QDBusConnection::sessionBus(), this))
    , m_dbusTrackPoint(new TrackPoint(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this))
    , m_dbusDevices(new InputDevices(Service, "/com/deepin/daemon/InputDevices", QDBusConnection::sessionBus(), this))
    , m_worker(worker)
{
    m_dbusMouse->setSync(false);
    m_dbusTouchPad->setSync(false);
    m_dbusTrackPoint->setSync(false);
    m_dbusDevices->setSync(false);

    init();
}

void MouseDBusProxy::active()
{
    // initial mouse settings
    m_worker->setMouseExist(m_dbusMouse->exist());
    m_worker->setLeftHandState(m_dbusMouse->leftHanded());
    m_worker->setMouseNaturalScrollState(m_dbusMouse->naturalScroll());
    m_worker->setDouClick(m_dbusMouse->doubleClick());
    m_worker->setDisTouchPad(m_dbusMouse->disableTpad());
    m_worker->setAccelProfile(m_dbusMouse->adaptiveAccelProfile());
    m_worker->setMouseMotionAcceleration(m_dbusMouse->motionAcceleration());

    // initial touchpad settings
    m_worker->setTouchpadMotionAcceleration(m_dbusTouchPad->motionAcceleration());
    m_worker->setTapClick(m_dbusTouchPad->tapClick());
    m_worker->setTpadExist(m_dbusTouchPad->exist());
    m_worker->setTouchNaturalScrollState(m_dbusTouchPad->naturalScroll());
    m_worker->setDisTyping(m_dbusTouchPad->disableIfTyping());
    m_worker->setPalmDetect(m_dbusTouchPad->palmDetect());
    m_worker->setPalmMinWidth(m_dbusTouchPad->palmMinWidth());
    m_worker->setPalmMinz(m_dbusTouchPad->palmMinZ());

    // initial redpoint settings
    m_worker->setTrackPointMotionAcceleration(m_dbusTrackPoint->motionAcceleration());
    m_worker->setRedPointExist(m_dbusTrackPoint->exist());

    // initial device properties
    m_worker->setScrollSpeed(m_dbusDevices->wheelSpeed());
}

void MouseDBusProxy::deactive()
{
    m_dbusMouse->blockSignals(true);
    m_dbusTouchPad->blockSignals(true);
    m_dbusTrackPoint->blockSignals(true);
}

void MouseDBusProxy::init()
{
    // get mouse settings from daemon
    connect(m_dbusMouse, &Mouse::ExistChanged, m_worker, &MouseWorker::setMouseExist);
    connect(m_dbusMouse, &Mouse::LeftHandedChanged, m_worker, &MouseWorker::setLeftHandState);
    connect(m_dbusMouse, &Mouse::NaturalScrollChanged, m_worker, &MouseWorker::setMouseNaturalScrollState);
    connect(m_dbusMouse, &Mouse::DoubleClickChanged, m_worker, &MouseWorker::setDouClick);
    connect(m_dbusMouse, &Mouse::DisableTpadChanged, m_worker, &MouseWorker::setDisTouchPad);
    connect(m_dbusMouse, &Mouse::AdaptiveAccelProfileChanged, m_worker, &MouseWorker::setAccelProfile);
    connect(m_dbusMouse, &Mouse::MotionAccelerationChanged, m_worker, &MouseWorker::setMouseMotionAcceleration);

    // get touchpad settings from daemon
    connect(m_dbusTouchPad, &TouchPad::ExistChanged, m_worker, &MouseWorker::setTpadExist);
    connect(m_dbusTouchPad, &TouchPad::NaturalScrollChanged, m_worker, &MouseWorker::setTouchNaturalScrollState);
    connect(m_dbusTouchPad, &TouchPad::DisableIfTypingChanged, m_worker, &MouseWorker::setDisTyping);
    connect(m_dbusTouchPad, &TouchPad::DoubleClickChanged, m_worker, &MouseWorker::setDouClick);
    connect(m_dbusTouchPad, &TouchPad::MotionAccelerationChanged, m_worker, &MouseWorker::setTouchpadMotionAcceleration);
    connect(m_dbusTouchPad, &TouchPad::TapClickChanged, m_worker, &MouseWorker::setTapClick);
    connect(m_dbusTouchPad, &TouchPad::PalmDetectChanged, m_worker, &MouseWorker::setPalmDetect);
    connect(m_dbusTouchPad, &TouchPad::PalmMinWidthChanged, m_worker, &MouseWorker::setPalmMinWidth);
    connect(m_dbusTouchPad, &TouchPad::PalmMinZChanged, m_worker, &MouseWorker::setPalmMinz);

    // get redpoint settings from daemon
    connect(m_dbusTrackPoint, &TrackPoint::ExistChanged, m_worker, &MouseWorker::setRedPointExist);
    connect(m_dbusTrackPoint, &TrackPoint::MotionAccelerationChanged, m_worker, &MouseWorker::setTrackPointMotionAcceleration);

    // get devices properties from daemon
    connect(m_dbusDevices, &InputDevices::WheelSpeedChanged, m_worker, &MouseWorker::setScrollSpeed);

    // set Mouse settings from dde-control-center
    connect(m_worker, &MouseWorker::requestSetLeftHandState, this, &MouseDBusProxy::setLeftHandState);
    connect(m_worker, &MouseWorker::requestSetMouseNaturalScrollState, this, &MouseDBusProxy::setMouseNaturalScrollState);
    connect(m_worker, &MouseWorker::requestSetDouClick, this, &MouseDBusProxy::setDouClick);
    connect(m_worker, &MouseWorker::requestSetDisTouchPad, this, &MouseDBusProxy::setDisTouchPad);
    connect(m_worker, &MouseWorker::requestSetAccelProfile, this, &MouseDBusProxy::setAccelProfile);
    connect(m_worker, &MouseWorker::requestSetMouseMotionAcceleration, this, &MouseDBusProxy::setMouseMotionAcceleration);

    // set Touchpad settings from dde-control-center
    connect(m_worker, &MouseWorker::requestSetTouchNaturalScrollState, this, &MouseDBusProxy::setTouchNaturalScrollState);
    connect(m_worker, &MouseWorker::requestSetDisTyping, this, &MouseDBusProxy::setDisTyping);
    connect(m_worker, &MouseWorker::requestSetTouchpadMotionAcceleration, this, &MouseDBusProxy::setTouchpadMotionAcceleration);
    connect(m_worker, &MouseWorker::requestSetTapClick, this, &MouseDBusProxy::setTapClick);
    connect(m_worker, &MouseWorker::requestSetPalmDetect, this, &MouseDBusProxy::setPalmDetect);
    connect(m_worker, &MouseWorker::requestSetPalmMinWidth, this, &MouseDBusProxy::setPalmMinWidth);
    connect(m_worker, &MouseWorker::requestSetPalmMinz, this, &MouseDBusProxy::setPalmMinz);

    // set Redpoint settings from dde-control-center
    connect(m_worker, &MouseWorker::requestSetTrackPointMotionAcceleration, this, &MouseDBusProxy::setTrackPointMotionAcceleration);

    // set Device properties from dde-control-center
    connect(m_worker, &MouseWorker::requestSetScrollSpeed, this, &MouseDBusProxy::setScrollSpeed);
}

void MouseDBusProxy::onDefaultReset()
{
    m_dbusMouse->Reset();
    m_dbusTouchPad->Reset();
    m_dbusTrackPoint->Reset();
}

void MouseDBusProxy::setLeftHandState(const bool state)
{
    m_dbusMouse->setLeftHanded(state);
    m_dbusTouchPad->setLeftHanded(state);
    m_dbusTrackPoint->setLeftHanded(state);
}

void MouseDBusProxy::setDouClick(const int &value)
{
    m_dbusMouse->setDoubleClick(value);
    m_dbusTouchPad->setDoubleClick(value);
}

void MouseDBusProxy::setMouseNaturalScrollState(const bool state)
{
    m_dbusMouse->setNaturalScroll(state);
}

void MouseDBusProxy::setDisTouchPad(const bool state)
{
    m_dbusMouse->setDisableTpad(state);
}

void MouseDBusProxy::setAccelProfile(const bool state)
{
    m_dbusMouse->setAdaptiveAccelProfile(state);
}

void MouseDBusProxy::setMouseMotionAcceleration(const double &value)
{
    m_dbusMouse->setMotionAcceleration(value);
}

void MouseDBusProxy::setTouchNaturalScrollState(const bool state)
{
    m_dbusTouchPad->setNaturalScroll(state);
}

void MouseDBusProxy::setDisTyping(const bool state)
{
    m_dbusTouchPad->setDisableIfTyping(state);
}

void MouseDBusProxy::setTouchpadMotionAcceleration(const double &value)
{
    m_dbusTouchPad->setMotionAcceleration(value);
}

void MouseDBusProxy::setTapClick(const bool state)
{
    m_dbusTouchPad->setTapClick(state);
}

void MouseDBusProxy::setPalmDetect(bool palmDetect)
{
    m_dbusTouchPad->setPalmDetect(palmDetect);
}

void MouseDBusProxy::setPalmMinWidth(int palmMinWidth)
{
    m_dbusTouchPad->setPalmMinWidth(palmMinWidth);
}

void MouseDBusProxy::setPalmMinz(int palmMinz)
{
    m_dbusTouchPad->setPalmMinZ(palmMinz);
}

void MouseDBusProxy::setTrackPointMotionAcceleration(const double &value)
{
    m_dbusTrackPoint->setMotionAcceleration(value);
}

void MouseDBusProxy::setScrollSpeed(uint speed)
{
    m_dbusDevices->setWheelSpeed(speed);
}
