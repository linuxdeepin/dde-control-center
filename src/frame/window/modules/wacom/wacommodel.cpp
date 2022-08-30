// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
