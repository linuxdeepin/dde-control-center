#include "mouseworker.h"

const QString Service = "com.deepin.daemon.InputDevices";

MouseWorker::MouseWorker(MouseModel *model, QObject *parent) :
    QObject(parent),
    m_dbusMouse(new Mouse(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this)),
    m_dbusTouchPad(new TouchPad(Service, "/com/deepin/daemon/InputDevice/TouchPad", QDBusConnection::sessionBus(), this)),
    m_dbusTrackPoint(new TrackPoint(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this)),
    m_model(model)
{

    m_dbusMouse->setSync(false);
    m_dbusTouchPad->setSync(false);
    m_dbusTouchPad->setSync(false);

    connect(m_dbusMouse, &Mouse::LeftHandedChanged, this, &MouseWorker::setLeftHandState);
    connect(m_dbusTouchPad, &TouchPad::NaturalScrollChanged, this, &MouseWorker::setNaturalScrollState);
    connect(m_dbusTouchPad, &TouchPad::DisableIfTypingChanged, this, &MouseWorker::setDisTyping);
    connect(m_dbusMouse, &Mouse::DoubleClickChanged, this, &MouseWorker::setDouClick);
    connect(m_dbusMouse, &Mouse::MotionAccelerationChanged, this, &MouseWorker::setMouseMotionAcceleration);
    connect(m_dbusMouse, &Mouse::DisableTpadChanged, this, &MouseWorker::setDisTouchPad);
    connect(m_dbusTouchPad, &TouchPad::MotionAccelerationChanged, this, &MouseWorker::setTouchpadMotionAcceleration);
    connect(m_dbusTouchPad, &TouchPad::TapClickChanged, this, &MouseWorker::setTapClick);
    connect(m_dbusTrackPoint, &TrackPoint::MotionAccelerationChanged, this, &MouseWorker::setTrackPointMotionAcceleration);
    active();

}

void MouseWorker::active()
{
    m_dbusMouse->blockSignals(false);
    m_dbusTouchPad->blockSignals(false);
    m_dbusTrackPoint->blockSignals(false);

    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setSliderValue(m_dbusMouse->doubleClick());
    modelBase->setLeftHandState(m_dbusMouse->leftHanded());
    modelBase->setNaturalScroll(m_dbusMouse->naturalScroll());
    modelBase->setDisIfTyping(m_dbusTouchPad->disableIfTyping());

    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    modelMouse->setSliderValue(std::round(3.2 - m_dbusMouse->motionAcceleration()) * 1000);
    modelMouse->setSwitchState(m_dbusMouse->disableTpad());

    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setSliderValue(std::round(3.2 - m_dbusTouchPad->motionAcceleration()) * 1000);
    modelTouch->setSwitchState(m_dbusTouchPad->tapClick());

    MouseModelThinkpadSettings *modelTrack = m_model->getTrackSettings();
    modelTrack->setSliderValue(std::round(3.2 - m_dbusTrackPoint->motionAcceleration()) * 1000);
}

void MouseWorker::deactive()
{
    m_dbusMouse->blockSignals(true);
    m_dbusTouchPad->blockSignals(true);
    m_dbusTrackPoint->blockSignals(true);
}

void MouseWorker::setLeftHandState(const bool state)
{
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setLeftHandState(state);
}

void MouseWorker::setNaturalScrollState(const bool state)
{
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setNaturalScroll(state);
}

void MouseWorker::setDisTyping(const bool state)
{
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setDisIfTyping(state);
}

void MouseWorker::setDisTouchPad(const bool state)
{

    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    modelMouse->setSwitchState(state);
}

void MouseWorker::setTapClick(const bool state)
{
    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setSwitchState(state);
}

void MouseWorker::setDouClick(const int &value)
{
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setSliderValue(value);
}

void MouseWorker::setMouseMotionAcceleration(const int &value)
{
    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    modelMouse->setSliderValue(std::round(3.2 - value) * 1000);
}

void MouseWorker::setTouchpadMotionAcceleration(const int &value)
{
    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setSliderValue(std::round(3.2 - value) * 1000);
}

void MouseWorker::setTrackPointMotionAcceleration(const int &value)
{
    MouseModelThinkpadSettings *modelTrack = m_model->getTrackSettings();
    modelTrack->setSliderValue(std::round(3.2 - value) * 1000);
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
}

void MouseWorker::onNaturalScrollStateChanged(const bool state)
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
    m_dbusTouchPad->setDoubleClick(value);
}

void MouseWorker::onMouseMotionAccelerationChanged(const int &value)
{
    m_dbusMouse->setMotionAcceleration(3.2 - (double)value / 1000);
}

void MouseWorker::onTouchpadMotionAccelerationChanged(const int &value)
{
    m_dbusTouchPad->setMotionAcceleration(3.2 - (double)value / 1000);
}

void MouseWorker::onTrackPointMotionAccelerationChanged(const int &value)
{
    m_dbusTrackPoint->setMotionAcceleration(3.2 - (double)value / 1000);
}

