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

#include "mousemodel.h"

using namespace dcc;
using namespace dcc::mouse;
MouseModel::MouseModel(QObject *parent)
    : QObject(parent)
    , m_leftHandState(false)
    , m_disIfTyping(false)
    , m_tpadExist(false)
    , m_mouseExist(true)
    , m_redPointExist(false)
    , m_mouseNaturalScroll(false)
    , m_tpadNaturalScroll(false)
    , m_disTpad(false)
    , m_palmDetect(false)
    , m_tapclick(false)
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

    emit leftHandStateChanged(state);
}

void MouseModel::setDisIfTyping(const bool state)
{
    if (m_disIfTyping == state)
        return;

    m_disIfTyping = state;

    emit disIfTypingStateChanged(state);
}

void MouseModel::setTpadExist(bool tpadExist)
{
    if (m_tpadExist == tpadExist)
        return;

    m_tpadExist = tpadExist;

    emit tpadExistChanged(tpadExist);
}

void MouseModel::setMouseExist(bool mouseExist)
{
    if (m_mouseExist == mouseExist)
        return;

    m_mouseExist = mouseExist;

    emit mouseExistChanged(mouseExist);
}

void MouseModel::setRedPointExist(bool redPointExist)
{
    if (m_redPointExist == redPointExist)
        return;

    m_redPointExist = redPointExist;

    emit redPointExistChanged(redPointExist);
}

void MouseModel::setDoubleSpeed(int doubleSpeed)
{
    if (m_doubleSpeed == doubleSpeed)
        return;

    m_doubleSpeed = doubleSpeed;

    emit doubleSpeedChanged(doubleSpeed);
}

void MouseModel::setMouseNaturalScroll(bool mouseNaturalScroll)
{
    if (m_mouseNaturalScroll == mouseNaturalScroll)
        return;

    m_mouseNaturalScroll = mouseNaturalScroll;

    emit mouseNaturalScrollChanged(mouseNaturalScroll);
}

void MouseModel::setTpadNaturalScroll(bool tpadNaturalScroll)
{
    if (m_tpadNaturalScroll == tpadNaturalScroll)
        return;

    m_tpadNaturalScroll = tpadNaturalScroll;

    emit tpadNaturalScrollChanged(tpadNaturalScroll);
}

void MouseModel::setMouseMoveSpeed(int mouseMoveSpeed)
{
    if (m_mouseMoveSpeed == mouseMoveSpeed)
        return;

    m_mouseMoveSpeed = mouseMoveSpeed;

    emit mouseMoveSpeedChanged(mouseMoveSpeed);
}

void MouseModel::setTpadMoveSpeed(int tpadMoveSpeed)
{
    if (m_tpadMoveSpeed == tpadMoveSpeed)
        return;

    m_tpadMoveSpeed = tpadMoveSpeed;

    emit tpadMoveSpeedChanged(tpadMoveSpeed);
}

void MouseModel::setDisTpad(bool disTpad)
{
    if (m_disTpad == disTpad)
        return;

    m_disTpad = disTpad;

    emit disTpadChanged(disTpad);
}

void MouseModel::setRedPointMoveSpeed(int redPointMoveSpeed)
{
    if (m_redPointMoveSpeed == redPointMoveSpeed)
        return;

    m_redPointMoveSpeed = redPointMoveSpeed;

    emit redPointMoveSpeedChanged(redPointMoveSpeed);
}

void MouseModel::setPalmDetect(bool palmDetect)
{
    if (m_palmDetect == palmDetect)
        return;

    m_palmDetect = palmDetect;

    emit palmDetectChanged(palmDetect);
}

void MouseModel::setPalmMinWidth(int palmMinWidth)
{
    if (m_palmMinWidth == palmMinWidth)
        return;

    m_palmMinWidth = palmMinWidth;

    emit palmMinWidthChanged(palmMinWidth);
}

void MouseModel::setPalmMinz(int palmMinz)
{
    if (m_palmMinz == palmMinz)
        return;

    m_palmMinz = palmMinz;

    emit palmMinzChanged(palmMinz);
}

void MouseModel::setTapClick(bool tapclick)
{
    if (m_tapclick == tapclick)
        return;

    m_tapclick = tapclick;

    emit tapClickChanged(tapclick);
}

void MouseModel::setScrollSpeed(int speed)
{
    if (m_scrollSpeed == speed)
        return;

    m_scrollSpeed = speed;

    emit scrollSpeedChanged(speed);
}
