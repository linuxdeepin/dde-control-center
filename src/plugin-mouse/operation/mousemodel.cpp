//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mousemodel.h"
#include "dccfactory.h"
#include "mouseworker.h"
#include "mousedbusproxy.h"

using namespace DCC_NAMESPACE;
MouseModel::MouseModel(QObject *parent)
    : QObject(parent)
    , m_leftHandState(false)
    , m_disIfTyping(false)
    , m_tpadExist(false)
    , m_mouseExist(true)
    , m_redPointExist(false)
    , m_mouseNaturalScroll(false)
    , m_tpadNaturalScroll(false)
    , m_accelProfile(true)
    , m_disTpad(false)
    , m_palmDetect(false)
    , m_tapClick(false)
    , m_touchpadEnabled(true)
    , m_doubleSpeed(1)
    , m_mouseMoveSpeed(1)
    , m_tpadMoveSpeed(1)
    , m_redPointMoveSpeed(1)
    , m_palmMinWidth(1)
    , m_palmMinz(100)
    , m_scrollSpeed(1)
    , m_worker(new MouseWorker(this, this))
{
}

MouseModel::~MouseModel()
{

}

void MouseModel::setLeftHandState(const bool state)
{
    if (m_leftHandState == state)
        return;

    m_leftHandState = state;

    QMetaObject::invokeMethod(m_worker,"onLeftHandStateChanged", Qt::QueuedConnection, m_leftHandState);
    Q_EMIT leftHandStateChanged(state);
}

void MouseModel::setDisIfTyping(const bool state)
{
    if (m_disIfTyping == state)
        return;

    m_disIfTyping = state;

    Q_EMIT disIfTypingStateChanged(state);
}

void MouseModel::setTpadExist(bool tpadExist)
{
    if (m_tpadExist == tpadExist)
        return;

    m_tpadExist = tpadExist;

    Q_EMIT tpadExistChanged(tpadExist);
}

void MouseModel::setMouseExist(bool mouseExist)
{
    if (m_mouseExist == mouseExist)
        return;

    m_mouseExist = mouseExist;

    Q_EMIT mouseExistChanged(mouseExist);
}

void MouseModel::setRedPointExist(bool redPointExist)
{
    if (m_redPointExist == redPointExist)
        return;

    m_redPointExist = redPointExist;

    Q_EMIT redPointExistChanged(redPointExist);
}

void MouseModel::setDoubleSpeed(int doubleSpeed)
{
    if (m_doubleSpeed == doubleSpeed)
        return;

    m_doubleSpeed = doubleSpeed;

    QMetaObject::invokeMethod(m_worker,"onDouClickChanged", Qt::QueuedConnection, m_doubleSpeed);
    Q_EMIT doubleSpeedChanged(doubleSpeed);
}

void MouseModel::setMouseNaturalScroll(bool mouseNaturalScroll)
{
    if (m_mouseNaturalScroll == mouseNaturalScroll)
        return;

    m_mouseNaturalScroll = mouseNaturalScroll;

    QMetaObject::invokeMethod(m_worker,"onMouseNaturalScrollStateChanged", Qt::QueuedConnection, m_mouseNaturalScroll);
    Q_EMIT mouseNaturalScrollChanged(mouseNaturalScroll);
}

void MouseModel::setTpadNaturalScroll(bool tpadNaturalScroll)
{
    if (m_tpadNaturalScroll == tpadNaturalScroll)
        return;

    m_tpadNaturalScroll = tpadNaturalScroll;

    QMetaObject::invokeMethod(m_worker,"onTouchNaturalScrollStateChanged", Qt::QueuedConnection, m_tpadNaturalScroll);
    Q_EMIT tpadNaturalScrollChanged(tpadNaturalScroll);
}

void MouseModel::setMouseMoveSpeed(int mouseMoveSpeed)
{
    if (m_mouseMoveSpeed == mouseMoveSpeed)
        return;

    m_mouseMoveSpeed = mouseMoveSpeed;

    QMetaObject::invokeMethod(m_worker,"onMouseMotionAccelerationChanged", Qt::QueuedConnection, m_mouseMoveSpeed);
    Q_EMIT mouseMoveSpeedChanged(mouseMoveSpeed);
}

void MouseModel::setTpadMoveSpeed(int tpadMoveSpeed)
{
    if (m_tpadMoveSpeed == tpadMoveSpeed)
        return;

    m_tpadMoveSpeed = tpadMoveSpeed;

    QMetaObject::invokeMethod(m_worker,"onTouchpadMotionAccelerationChanged", Qt::QueuedConnection, m_tpadMoveSpeed);
    Q_EMIT tpadMoveSpeedChanged(tpadMoveSpeed);
}

void MouseModel::setAccelProfile(bool useAdaptiveProfile)
{
    if (m_accelProfile == useAdaptiveProfile)
        return;

    m_accelProfile = useAdaptiveProfile;

    QMetaObject::invokeMethod(m_worker,"onAccelProfileChanged", Qt::QueuedConnection, m_accelProfile);
    Q_EMIT accelProfileChanged(useAdaptiveProfile);
}

void MouseModel::setDisTpad(bool disTpad)
{
    if (m_disTpad == disTpad)
        return;

    m_disTpad = disTpad;

    QMetaObject::invokeMethod(m_worker,"onDisTouchPadChanged", Qt::QueuedConnection, m_disTpad);
    Q_EMIT disTpadChanged(disTpad);
}

void MouseModel::setRedPointMoveSpeed(int redPointMoveSpeed)
{
    if (m_redPointMoveSpeed == redPointMoveSpeed)
        return;

    m_redPointMoveSpeed = redPointMoveSpeed;

    Q_EMIT redPointMoveSpeedChanged(redPointMoveSpeed);
}

void MouseModel::setPalmDetect(bool palmDetect)
{
    if (m_palmDetect == palmDetect)
        return;

    m_palmDetect = palmDetect;

    Q_EMIT palmDetectChanged(palmDetect);
}

void MouseModel::setPalmMinWidth(int palmMinWidth)
{
    if (m_palmMinWidth == palmMinWidth)
        return;

    m_palmMinWidth = palmMinWidth;

    Q_EMIT palmMinWidthChanged(palmMinWidth);
}

void MouseModel::setPalmMinz(int palmMinz)
{
    if (m_palmMinz == palmMinz)
        return;

    m_palmMinz = palmMinz;

    Q_EMIT palmMinzChanged(palmMinz);
}

void MouseModel::setTapClick(bool tapClick)
{
    if (m_tapClick == tapClick)
        return;

    m_tapClick = tapClick;

    QMetaObject::invokeMethod(m_worker,"onTapClick", Qt::QueuedConnection, m_tapClick);
    Q_EMIT tapClickChanged(tapClick);
}

void MouseModel::setTapEnabled(bool tabEnabled)
{
    if (m_touchpadEnabled == tabEnabled)
        return;

    m_touchpadEnabled = tabEnabled;

    Q_EMIT tapEnabledChanged(tabEnabled);
}

void MouseModel::setScrollSpeed(int speed)
{
    if (m_scrollSpeed == speed)
        return;

    m_scrollSpeed = speed;

    QMetaObject::invokeMethod(m_worker,"onScrollSpeedChanged", Qt::QueuedConnection, m_scrollSpeed);
    Q_EMIT scrollSpeedChanged(speed);
}
DCC_FACTORY_CLASS(MouseModel)

#include "mousemodel.moc"
