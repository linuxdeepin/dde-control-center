//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mousemodel.h"

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
    , m_tapclick(false)
    , m_touchpadEnabled(true)
    , m_doubleSpeed(1)
    , m_mouseMoveSpeed(1)
    , m_tpadMoveSpeed(1)
    , m_redPointMoveSpeed(1)
    , m_palmMinWidth(1)
    , m_palmMinz(100)
    , m_scrollSpeed(1)
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

    Q_EMIT doubleSpeedChanged(doubleSpeed);
}

void MouseModel::setMouseNaturalScroll(bool mouseNaturalScroll)
{
    if (m_mouseNaturalScroll == mouseNaturalScroll)
        return;

    m_mouseNaturalScroll = mouseNaturalScroll;

    Q_EMIT mouseNaturalScrollChanged(mouseNaturalScroll);
}

void MouseModel::setTpadNaturalScroll(bool tpadNaturalScroll)
{
    if (m_tpadNaturalScroll == tpadNaturalScroll)
        return;

    m_tpadNaturalScroll = tpadNaturalScroll;

    Q_EMIT tpadNaturalScrollChanged(tpadNaturalScroll);
}

void MouseModel::setMouseMoveSpeed(int mouseMoveSpeed)
{
    if (m_mouseMoveSpeed == mouseMoveSpeed)
        return;

    m_mouseMoveSpeed = mouseMoveSpeed;

    Q_EMIT mouseMoveSpeedChanged(mouseMoveSpeed);
}

void MouseModel::setTpadMoveSpeed(int tpadMoveSpeed)
{
    if (m_tpadMoveSpeed == tpadMoveSpeed)
        return;

    m_tpadMoveSpeed = tpadMoveSpeed;

    Q_EMIT tpadMoveSpeedChanged(tpadMoveSpeed);
}

void MouseModel::setAccelProfile(bool useAdaptiveProfile)
{
    if (m_accelProfile == useAdaptiveProfile)
        return;

    m_accelProfile = useAdaptiveProfile;

    Q_EMIT accelProfileChanged(useAdaptiveProfile);
}

void MouseModel::setDisTpad(bool disTpad)
{
    if (m_disTpad == disTpad)
        return;

    m_disTpad = disTpad;

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

void MouseModel::setTapClick(bool tapclick)
{
    if (m_tapclick == tapclick)
        return;

    m_tapclick = tapclick;

    Q_EMIT tapClickChanged(tapclick);
}

void MouseModel::setTapEnabled(bool tabEnabled)
{
    if (m_touchpadEnabled == tabEnabled)
        return;

    m_touchpadEnabled = tabEnabled;

    Q_EMIT tapEnabledChanged(tabEnabled);
}

void MouseModel::setScrollSpeed(uint speed)
{
    if (m_scrollSpeed == speed)
        return;

    m_scrollSpeed = speed;

    Q_EMIT scrollSpeedChanged(speed);
}
