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

#include "wacomdbusproxy.h"
#include "widgets/dccdbusinterface.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const static QString WacomService = "org.deepin.dde.InputDevices1";
const static QString WacomPath = "/org/deepin/dde/InputDevice1/Wacom";
const static QString WacomInterface = "org.deepin.dde.InputDevice1.Wacom";

using namespace DCC_NAMESPACE;

WacomDBusProxy::WacomDBusProxy(QObject *parent)
    : QObject (parent)
    , m_inputWacomInter(new DCCDBusInterface(WacomService, WacomPath, WacomInterface, QDBusConnection::sessionBus(), this))
{

}

bool WacomDBusProxy::exist()
{
    return qvariant_cast<bool>(m_inputWacomInter->property("Exist"));
}

uint WacomDBusProxy::stylusPressureSensitive()
{
    return qvariant_cast<uint>(m_inputWacomInter->property("StylusPressureSensitive"));
}

void WacomDBusProxy::setStylusPressureSensitive(uint value)
{
    m_inputWacomInter->setProperty("StylusPressureSensitive", QVariant::fromValue(value));
}

bool WacomDBusProxy::cursorMode()
{
    return qvariant_cast<bool>(m_inputWacomInter->property("CursorMode"));
}

void WacomDBusProxy::setCursorMode(bool value)
{
    m_inputWacomInter->setProperty("CursorMode", value);
}

uint WacomDBusProxy::eraserPressureSensitive()
{
    return qvariant_cast<bool>(m_inputWacomInter->property("EraserPressureSensitive"));
}

void WacomDBusProxy::setEraserPressureSensitive(uint value)
{
    m_inputWacomInter->setProperty("EraserPressureSensitive", value);
}
