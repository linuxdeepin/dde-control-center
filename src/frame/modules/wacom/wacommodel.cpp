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

#include "wacommodel.h"
#include "model/wacommodelbase.h"
#include <QObject>
#include <QDebug>
using namespace dcc;
using namespace dcc::wacom;

WacomModel::WacomModel(QObject *parent)
    : QObject(parent),

      m_exist(false)
{
    m_wacomModelBase = new WacomModelBase;
}

WacomModel::~WacomModel()
{
    m_wacomModelBase->deleteLater();
}

WacomModelBase *WacomModel::getWacomModelBase()
{
    return m_wacomModelBase;
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
