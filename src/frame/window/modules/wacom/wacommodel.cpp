/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "wacommodel.h"

#include <QDebug>

namespace  DCC_NAMESPACE {
namespace wacom {

WacomModel::WacomModel(QObject *parent)
    : QObject(parent)
    , m_exist(false)
    , m_cursorMode(false)
    , m_pressureValue(0)
{
}

WacomModel::~WacomModel()
{
}

bool WacomModel::exist() const
{
    return m_exist;
}

void WacomModel::setExist(bool exist)
{
    if (m_exist != exist) {
        m_exist = exist;
        Q_EMIT existChanged(exist);
    }
}

bool WacomModel::getCursorMode() const
{
    return m_cursorMode;
}

void WacomModel::setCursorMode(bool cursorMode)
{
    if (m_cursorMode == cursorMode)
        return;

    m_cursorMode = cursorMode;
    Q_EMIT cursorModeChanged(cursorMode);
}

void WacomModel::setPressureValue(const uint value)
{
    if (m_pressureValue == value)
        return;

    m_pressureValue = value;

    Q_EMIT pressureValueChanged(value);
}

uint WacomModel::getPressureValue()
{
    return m_pressureValue;
}

}
}
