#include "mouseworker.h"
using namespace dcc;
using namespace dcc::mouse;
const QString Service = "com.deepin.daemon.InputDevices";

MouseWorker::MouseWorker(MouseModel *model, QObject *parent) :
    QObject(parent),
    m_dbusMouse(new Mouse(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this)),
    m_dbusTouchPad(new TouchPad(Service, "/com/deepin/daemon/InputDevice/TouchPad", QDBusConnection::sessionBus(), this)),
    m_dbusTrackPoint(new TrackPoint(Service, "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this)),
    m_model(model)
{
    connect(m_dbusMouse, &Mouse::LeftHandedChanged, this, &MouseWorker::setLeftHandState);
    connect(m_dbusMouse, &Mouse::NaturalScrollChanged, this, &MouseWorker::setMouseNaturalScrollState);
    connect(m_dbusTouchPad, &TouchPad::NaturalScrollChanged, this, &MouseWorker::setTouchNaturalScrollState);
    connect(m_dbusTouchPad, &TouchPad::DisableIfTypingChanged, this, &MouseWorker::setDisTyping);
    connect(m_dbusMouse, &Mouse::DoubleClickChanged, this, &MouseWorker::setDouClick);
    connect(m_dbusTouchPad, &TouchPad::DoubleClickChanged, this, &MouseWorker::setDouClick);
    connect(m_dbusMouse, &Mouse::MotionAccelerationChanged, this, &MouseWorker::setMouseMotionAcceleration);
    connect(m_dbusMouse, &Mouse::DisableTpadChanged, this, &MouseWorker::setDisTouchPad);
    connect(m_dbusTouchPad, &TouchPad::MotionAccelerationChanged, this, &MouseWorker::setTouchpadMotionAcceleration);
    connect(m_dbusTouchPad, &TouchPad::TapClickChanged, this, &MouseWorker::setTapClick);
    connect(m_dbusTrackPoint, &TrackPoint::MotionAccelerationChanged, this, &MouseWorker::setTrackPointMotionAcceleration);

    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    connect(m_dbusMouse, &Mouse::ExistChanged, modelMouse, &MouseModelMouseSettings::setExist);

    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    connect(m_dbusTouchPad, &TouchPad::ExistChanged, modelTouch, &MouseModelMouseSettings::setExist);

    MouseModelThinkpadSettings *modelTrack = m_model->getTrackSettings();
    connect(m_dbusTrackPoint, &TrackPoint::ExistChanged, modelTrack, &MouseModelThinkpadSettings::setExist);

    m_dbusMouse->setSync(false);
    m_dbusTouchPad->setSync(false);
    m_dbusTouchPad->setSync(false);

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
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setExist(m_dbusMouse->exist());

    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setExist(m_dbusTouchPad->exist());

    MouseModelThinkpadSettings *modelTrack = m_model->getTrackSettings();
    modelTrack->setExist(m_dbusTrackPoint->exist());

    setLeftHandState(m_dbusMouse->leftHanded());
    setMouseNaturalScrollState(m_dbusMouse->naturalScroll());
    setTouchNaturalScrollState(m_dbusTouchPad->naturalScroll());
    setDisTyping(m_dbusTouchPad->disableIfTyping());
    setDisTouchPad(m_dbusMouse->disableTpad());
    setTapClick(m_dbusTouchPad->tapClick());
    setDouClick(m_dbusMouse->doubleClick());
    setMouseMotionAcceleration(m_dbusMouse->motionAcceleration());
    setTouchpadMotionAcceleration(m_dbusTouchPad->motionAcceleration());
    setTrackPointMotionAcceleration(m_dbusTrackPoint->motionAcceleration());
}

void MouseWorker::setLeftHandState(const bool state)
{
    MouseModelBaseSettings *modelBase = m_model->getBaseSettings();
    modelBase->setLeftHandState(state);
}

void MouseWorker::setMouseNaturalScrollState(const bool state)
{
    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    modelMouse->setNaturalScroll(state);
}

void MouseWorker::setTouchNaturalScrollState(const bool state)
{
    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setNaturalScroll(state);
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
    modelBase->setSliderValue(converToDoubleModel(value));
}

void MouseWorker::setMouseMotionAcceleration(const double &value)
{
    MouseModelMouseSettings *modelMouse = m_model->getMouseSettings();
    modelMouse->setSliderValue(converToModelMotionAcceleration(value));
}

void MouseWorker::setTouchpadMotionAcceleration(const double &value)
{
    MouseModelMouseSettings *modelTouch = m_model->getTouchSettings();
    modelTouch->setSliderValue(converToModelMotionAcceleration(value));
}

void MouseWorker::setTrackPointMotionAcceleration(const double &value)
{
    MouseModelThinkpadSettings *modelTrack = m_model->getTrackSettings();
    modelTrack->setSliderValue(converToModelMotionAcceleration(value));
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

