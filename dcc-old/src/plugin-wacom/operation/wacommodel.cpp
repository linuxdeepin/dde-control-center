//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
