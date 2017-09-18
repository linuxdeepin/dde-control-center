/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include "model/mousemodelbase.h"
#include "model/mousemodelbasesettings.h"
#include "model/mousemodelmousesettings.h"
#include "model/mousemodelthinkpadsettings.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModel::MouseModel(QObject *parent)
    :QObject(parent)
{
    m_baseSettings  = new MouseModelBaseSettings(this);
    m_mouseSettings = new MouseModelMouseSettings(this);
    m_touchSettings = new MouseModelMouseSettings(this);
    m_trackSettings = new MouseModelThinkpadSettings(this);
}

MouseModel::~MouseModel()
{

}

MouseModelBaseSettings *MouseModel::getBaseSettings() const
{
    return m_baseSettings;
}

MouseModelMouseSettings *MouseModel::getMouseSettings() const
{
    return m_mouseSettings;
}

MouseModelMouseSettings *MouseModel::getTouchSettings() const
{
    return m_touchSettings;
}

MouseModelThinkpadSettings *MouseModel::getTrackSettings() const
{
    return m_trackSettings;
}
