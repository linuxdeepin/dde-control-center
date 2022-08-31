// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wacomworker.h"
#include "wacommodel.h"

namespace DCC_NAMESPACE {
namespace wacom {

const QString Service("com.deepin.daemon.InputDevices");
const QString ServicePath("/com/deepin/daemon/InputDevice/Wacom");

WacomWorker::WacomWorker(WacomModel *model, QObject *parent)
    : QObject(parent)
    , m_dbusWacom(new Wacom(Service, ServicePath, QDBusConnection::sessionBus(), this))
    , m_model(model)
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
    uint pressure = m_dbusWacom->stylusPressureSensitive();
    if (pressure < 1 || pressure > 7) {
        pressure = m_dbusWacom->stylusPressureSensitive();
    }
    m_model->setPressureValue(pressure);
    m_model->setExist(m_dbusWacom->exist());
    m_model->setCursorMode(m_dbusWacom->cursorMode());
}

void WacomWorker::deactive()
{
    m_dbusWacom->blockSignals(true);
}

void WacomWorker::setPressureSensitive(const uint value)
{
    m_model->setPressureValue(value);
}

void WacomWorker::setCursorMode(const bool value)
{
    m_dbusWacom->setCursorMode(value);
}

void WacomWorker::onPressureSensitiveChanged(const uint value)
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

}
}
