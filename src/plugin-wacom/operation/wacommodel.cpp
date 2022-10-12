/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "wacommodelprivate_p.h"

WacomModel::WacomModel(QObject *parent)
    : QObject(parent)
    , d_ptr(new WacomModelPrivate(this))
{
}

WacomModel::~WacomModel()
{

}

bool WacomModel::exist() const
{
    Q_D(const WacomModel);
    return d->m_exist;
}

bool WacomModel::CursorMode() const
{
    Q_D(const WacomModel);
    return d->m_cursorMode;
}

void WacomModel::setCursorMode(bool value)
{
    Q_D(WacomModel);
    d->setCursorMode(value);
}

uint WacomModel::eraserPressureSensitive()
{
    Q_D(const WacomModel);
    return d->m_pressureValue;
}

void WacomModel::setEraserPressureSensitive(uint value)
{
    Q_D(WacomModel);
    d->setEraserPressureSensitive(value);
}

void WacomModelPrivate::setCursorMode(bool value) {
    m_wacomInterfaceProxy->setCursorMode(value);
}

void WacomModelPrivate::setEraserPressureSensitive(uint value) {
    m_wacomInterfaceProxy->setEraserPressureSensitive(value);
}
