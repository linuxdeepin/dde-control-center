//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "wacomdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const static QString WacomService = "org.deepin.dde.InputDevices1";
const static QString WacomPath = "/org/deepin/dde/InputDevice1/Wacom";
const static QString WacomInterface = "org.deepin.dde.InputDevice1.Wacom";

WacomDBusProxy::WacomDBusProxy(QObject *parent)
    : QObject (parent)
    , m_inputWacomInter(new DDBusInterface(WacomService, WacomPath, WacomInterface, QDBusConnection::sessionBus(), this))
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
    return qvariant_cast<uint>(m_inputWacomInter->property("EraserPressureSensitive"));
}

void WacomDBusProxy::setEraserPressureSensitive(uint value)
{
    m_inputWacomInter->setProperty("EraserPressureSensitive", value);
}
