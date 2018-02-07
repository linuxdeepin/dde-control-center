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

#include "mousemodelbasesettings.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModelBaseSettings::MouseModelBaseSettings(QObject *parent)
    : MouseModelBase(parent)
    , m_leftHandState(false)
    , m_disIfTyping(false)
{

}

void MouseModelBaseSettings::setLeftHandState(const bool state)
{
    if (m_leftHandState != state) {
        m_leftHandState = state;
        emit leftHandStateChanged(state);
    }
}

void MouseModelBaseSettings::setDisIfTyping(const bool state)
{
    if (m_disIfTyping != state) {
        m_disIfTyping = state;
        emit disIfTypingStateChanged(state);
    }
}
