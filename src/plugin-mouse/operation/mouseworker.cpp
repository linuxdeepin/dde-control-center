//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mouseworker.h"

#include "mousedbusproxy.h"

using namespace DCC_NAMESPACE;
const QString Service = "org.deepin.dde.InputDevices1";

MouseWorker::MouseWorker(MouseModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_mouseProxy(new MouseDBusProxy(this))
    , m_treelandWorker(new TreeLandWorker(this))
{
    bindProxySignals();
    bindRequestSignals();
    QMetaObject::invokeMethod(m_mouseProxy, "active", Qt::QueuedConnection);
#ifdef Enable_Treeland
    m_treelandWorker->active();
#endif
}

void MouseWorker::active()
{
}

void MouseWorker::deactive()
{
}

void MouseWorker::init()
{
}

void MouseWorker::bindProxySignals()
{
    connect(m_mouseProxy, &MouseDBusProxy::mouseExistChanged, this, &MouseWorker::setMouseExist);
    connect(m_mouseProxy, &MouseDBusProxy::tpadExistChanged, this, &MouseWorker::setTpadExist);
    connect(m_mouseProxy, &MouseDBusProxy::tpadEnabledChanged, this, &MouseWorker::setTpadEnabled);
    connect(m_mouseProxy, &MouseDBusProxy::leftHandStateChanged, this, &MouseWorker::setLeftHandState);
    connect(m_mouseProxy, &MouseDBusProxy::mouseNaturalScrollStateChanged, this, &MouseWorker::setMouseNaturalScrollState);
    connect(m_mouseProxy, &MouseDBusProxy::touchNaturalScrollStateChanged, this, &MouseWorker::setTouchNaturalScrollState);
    connect(m_mouseProxy, &MouseDBusProxy::disTypingChanged, this, &MouseWorker::setDisTyping);
    connect(m_mouseProxy, &MouseDBusProxy::disTouchPadChanged, this, &MouseWorker::setDisTouchPad);
    connect(m_mouseProxy, &MouseDBusProxy::tapClickChanged, this, &MouseWorker::setTapClick);
    connect(m_mouseProxy, &MouseDBusProxy::douClickChanged, this, &MouseWorker::setDouClick);
    connect(m_mouseProxy, &MouseDBusProxy::mouseMotionAccelerationChanged, this, &MouseWorker::setMouseMotionAcceleration);
    connect(m_mouseProxy, &MouseDBusProxy::accelProfileChanged, this, &MouseWorker::setAccelProfile);
    connect(m_mouseProxy, &MouseDBusProxy::touchpadMotionAccelerationChanged, this, &MouseWorker::setTouchpadMotionAcceleration);
    connect(m_mouseProxy, &MouseDBusProxy::redPointExistChanged, this, &MouseWorker::setRedPointExist);
    connect(m_mouseProxy, &MouseDBusProxy::trackPointMotionAccelerationChanged, this, &MouseWorker::setTrackPointMotionAcceleration);
    connect(m_mouseProxy, &MouseDBusProxy::palmDetectChanged, this, &MouseWorker::setPalmDetect);
    connect(m_mouseProxy, &MouseDBusProxy::palmMinWidthChanged, this, &MouseWorker::setPalmMinWidth);
    connect(m_mouseProxy, &MouseDBusProxy::palmMinzChanged, this, &MouseWorker::setPalmMinz);
    connect(m_mouseProxy, &MouseDBusProxy::scrollSpeedChanged, this, &MouseWorker::setScrollSpeed);
    connect(m_mouseProxy, &MouseDBusProxy::gestureDataChanged, this, [this](const GestureData &data) {
        setGestureData(data);
        initFingerGestures();
    });
    connect(m_mouseProxy, &MouseDBusProxy::cursorSizeChanged, this, &MouseWorker::setCursorSize);
    connect(m_mouseProxy, &MouseDBusProxy::availableCursorSizesChanged, this, &MouseWorker::setAvailableCursorSizes);
    connect(m_mouseProxy, &MouseDBusProxy::lidIsPresentChanged, this, &MouseWorker::setLidIsPresent);
}

void MouseWorker::bindRequestSignals()
{
    connect(this, &MouseWorker::requestSetPalmDetect, m_mouseProxy, &MouseDBusProxy::setPalmDetect);
    connect(this, &MouseWorker::requestSetPalmMinWidth, m_mouseProxy, &MouseDBusProxy::setPalmMinWidth);
    connect(this, &MouseWorker::requestSetPalmMinz, m_mouseProxy, &MouseDBusProxy::setPalmMinz);
    connect(this, &MouseWorker::requestSetDouClick, m_mouseProxy, &MouseDBusProxy::setDouClick);
    connect(this, &MouseWorker::requestSetScrollSpeed, m_mouseProxy, &MouseDBusProxy::setScrollSpeed);
    connect(this, &MouseWorker::requestSetLeftHandState, m_mouseProxy, &MouseDBusProxy::setLeftHandState);
    connect(this, &MouseWorker::requestSetMouseNaturalScrollState, m_mouseProxy, &MouseDBusProxy::setMouseNaturalScrollState);
    connect(this, &MouseWorker::requestSetTouchNaturalScrollState, m_mouseProxy, &MouseDBusProxy::setTouchNaturalScrollState);
    connect(this, &MouseWorker::requestSetDisTyping, m_mouseProxy, &MouseDBusProxy::setDisTyping);
    connect(this, &MouseWorker::requestSetDisTouchPad, m_mouseProxy, &MouseDBusProxy::setDisableTouchPadWhenMouseExist);
    connect(this, &MouseWorker::requestSetTapClick, m_mouseProxy, &MouseDBusProxy::setTapClick);
    connect(this, &MouseWorker::requestSetMouseMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setMouseMotionAcceleration);
    connect(this, &MouseWorker::requestSetAccelProfile, m_mouseProxy, &MouseDBusProxy::setAccelProfile);
    connect(this, &MouseWorker::requestSetTouchpadMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setTouchpadMotionAcceleration);
    connect(this, &MouseWorker::requestSetTrackPointMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setTrackPointMotionAcceleration);
    connect(this, &MouseWorker::requestSetTouchpadEnabled, m_mouseProxy, &MouseDBusProxy::setTouchpadEnabled);
    connect(this, &MouseWorker::requestSetGesture, m_mouseProxy, &MouseDBusProxy::setGesture);
    connect(this, &MouseWorker::requestSetCursorSize, m_mouseProxy, &MouseDBusProxy::setCursorSize);
}

void MouseWorker::initFingerGestures()
{
    m_model->updateFigerAniPath();
}

void MouseWorker::setMouseExist(bool exist)
{
    m_model->setMouseExist(exist);
}

void MouseWorker::setTpadExist(bool exist)
{
    m_model->setTpadExist(exist);
}

void MouseWorker::setTpadEnabled(bool enabled)
{
    m_model->setTapEnabled(enabled);
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

void MouseWorker::setGestureData(const GestureData &data)
{
    m_model->updateGesturesData(data);
}

void MouseWorker::setCursorSize(const int cursorSize)
{
    m_model->setCursorSize(cursorSize);
}

void MouseWorker::setAvailableCursorSizes(QList<int> sizes)
{
    m_model->setAvailableCursorSizes(sizes);
}

void MouseWorker::setLidIsPresent(bool lidIsPresent)
{
    m_model->setLidIsPresent(lidIsPresent);
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

void MouseWorker::onTouchpadEnabledChanged(const bool state)
{
    Q_EMIT requestSetTouchpadEnabled(state);
}

void MouseWorker::onCursorSizeChanged(const int cursorSize)
{
    Q_EMIT requestSetCursorSize(cursorSize);
#ifdef Enable_Treeland
    if (Dtk::Gui::DGuiApplicationHelper::testAttribute(
                Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform)) {
        m_treelandWorker->setCursorSize(cursorSize);
    }
#endif
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
