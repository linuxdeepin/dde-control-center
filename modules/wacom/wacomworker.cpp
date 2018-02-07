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

#include "wacomworker.h"
#include "wacommodel.h"
#include "model/wacommodelbase.h"

using namespace dcc;
using namespace dcc::wacom;

const QString Service = "com.deepin.daemon.InputDevices";

WacomWorker::WacomWorker(WacomModel *model, QObject *parent) :
    QObject(parent),
    m_dbusWacom(new Wacom(Service, "/com/deepin/daemon/InputDevice/Wacom", QDBusConnection::sessionBus(), this)),
    m_model(model)
{
    connect(m_dbusWacom, &Wacom::StylusPressureSensitiveChanged, this, &WacomWorker::setPressureSensitive);
    connect(m_dbusWacom, &Wacom::ExistChanged, m_model, &WacomModel::setExist);
    connect(m_dbusWacom, &Wacom::CursorModeChanged, this, &WacomWorker::onCursorModeChanged);

    //get real data
    m_model->setExist(m_dbusWacom->exist());
    m_dbusWacom->setSync(false);
}

void WacomWorker::active()
{
    m_dbusWacom->blockSignals(false);

    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(m_dbusWacom->stylusPressureSensitive());
    m_model->setExist(m_dbusWacom->exist());
    m_model->setCursorMode(m_dbusWacom->cursorMode());
}

void WacomWorker::deactive()
{
    m_dbusWacom->blockSignals(true);
}

void WacomWorker::setPressureSensitive(const int value)
{
    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(value);
}

void WacomWorker::setCursorMode(const bool value)
{
    m_dbusWacom->setCursorMode(value);
}

void WacomWorker::onPressureSensitiveChanged(const int value)
{
    m_dbusWacom->setStylusPressureSensitive(value);
    m_dbusWacom->setEraserPressureSensitive(value);
}

void WacomWorker::onCursorModeChanged(const bool value)
{
    m_model->setCursorMode(value);
}

bool WacomWorker::exist()
{
    return m_dbusWacom->exist();
}
