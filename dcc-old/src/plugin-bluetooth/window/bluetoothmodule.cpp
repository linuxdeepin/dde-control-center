//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothmodule.h"
#include "bluetoothmodel.h"
#include "adaptermodule.h"
#include "bluetoothworker.h"
#include "bluetoothadapter.h"
#include "bluetoothdevice.h"
#include "pincodedialog.h"

#include <QDBusObjectPath>
#include <QApplication>
#include <QLoggingCategory>

#include <DIconTheme>
DGUI_USE_NAMESPACE

Q_LOGGING_CATEGORY(DdcBluetoothModule, "dcc-bluetooth-module")

using namespace DCC_NAMESPACE;

BluetoothModule::BluetoothModule(QObject *parent)
    : PageModule("bluetooth", tr("Bluetooth"), tr("Bluetooth device manager"), DIconTheme::findQIcon("dcc_nav_bluetooth"), parent)
{
    m_model = new BluetoothModel(this);
    m_work = new BluetoothWorker(m_model, this);

    connect(this, &BluetoothModule::requestSetToggleAdapter, m_work, &BluetoothWorker::setAdapterPowered);
    connect(this, &BluetoothModule::requestConnectDevice, m_work, &BluetoothWorker::connectDevice);
    connect(this, &BluetoothModule::requestDisconnectDevice, m_work, &BluetoothWorker::disconnectDevice);
    connect(this, &BluetoothModule::requestSetAlias, m_work, &BluetoothWorker::setAlias);
    connect(this, &BluetoothModule::requestDiscoverable, m_work, &BluetoothWorker::onRequestSetDiscoverable);

    connect(m_work, &BluetoothWorker::requestConfirmation, this, &BluetoothModule::showPinCode);
    connect(m_work, &BluetoothWorker::pinCodeCancel, this, &BluetoothModule::closePinCode);

    connect(m_model, &BluetoothModel::adapterAdded, this, &BluetoothModule::addAdapter);
    connect(m_model, &BluetoothModel::adapterRemoved, this, &BluetoothModule::removeAdapter);

    for (const BluetoothAdapter *adapter : m_model->adapters())
        addAdapter(adapter);

    updateWidget();
}

void BluetoothModule::active()
{
    for (auto &&module : m_valueMap)
        module->active();
}

void BluetoothModule::deactive()
{
    for (auto &&adapter : m_valueMap.keys())
        m_work->setAdapterDiscovering(QDBusObjectPath(adapter->id()), false);
}

bool BluetoothModule::hasDevice()
{
    return !m_valueMap.isEmpty();
}

void BluetoothModule::showPinCode(const QDBusObjectPath &device, const QString &code)
{
    qCDebug(DdcBluetoothModule) << "request confirmation: " << device.path() << code;
    PinCodeDialog *dialog = PinCodeDialog::instance(code);
    m_dialogs[device] = dialog;
    if (!dialog->isVisible()) {
        int ret = dialog->exec();
        closePinCode(device);
        m_work->pinCodeConfirm(device, bool(ret));
    }
}

void BluetoothModule::closePinCode(const QDBusObjectPath &device)
{
    PinCodeDialog *dialog = m_dialogs[device];
    m_dialogs.remove(device);
    QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
}

AdapterModule *BluetoothModule::getAdapter(const BluetoothAdapter *adapter)
{
    AdapterModule *adpWidget = new AdapterModule(adapter, m_model, m_work, this);

    const QDBusObjectPath path(adapter->id());

    connect(adpWidget, &AdapterModule::requestSetToggleAdapter, this, &BluetoothModule::requestSetToggleAdapter);
    connect(adpWidget, &AdapterModule::requestConnectDevice, this, &BluetoothModule::requestConnectDevice);
    connect(adpWidget, &AdapterModule::requestDisconnectDevice, this, &BluetoothModule::requestDisconnectDevice);
    connect(adpWidget, &AdapterModule::requestSetAlias, this, &BluetoothModule::requestSetAlias);
    connect(adpWidget, &AdapterModule::requestRefresh, this, &BluetoothModule::requestRefresh);
    connect(adpWidget, &AdapterModule::requestDiscoverable, this, &BluetoothModule::requestDiscoverable);
    connect(adpWidget, &AdapterModule::requestDiscoverable, this, &BluetoothModule::requestDiscoverable);
    connect(adpWidget, &AdapterModule::requestSetDevAlias, m_work, &BluetoothWorker::setDeviceAlias);
    connect(adpWidget, &AdapterModule::requestSetDisplaySwitch, m_work, &BluetoothWorker::setDisplaySwitch);
    connect(adpWidget, &AdapterModule::requestIgnoreDevice, m_work, &BluetoothWorker::ignoreDevice);

    return adpWidget;
}

void BluetoothModule::addAdapter(const BluetoothAdapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        m_valueMap[adapter] = getAdapter(adapter);
        updateVisible();
        updateWidget();
    }
}
void BluetoothModule::removeAdapter(const BluetoothAdapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        AdapterModule *adpModule = m_valueMap.take(adapter);
        for (auto &&obj : adpModule->ModuleList()) {
            removeChild(obj);
        }
        adpModule->setParent(nullptr);
        adpModule->deleteLater();
        updateWidget();
    }
}

void BluetoothModule::requestRefresh(const BluetoothAdapter *adapter)
{
    m_work->setAdapterDiscoverable(adapter->id());
}

void BluetoothModule::updateVisible()
{
    int row = 0;
    for (const BluetoothAdapter *adapter : m_model->adapters()) {
        auto &&adpModule = m_valueMap.value(adapter, nullptr);
        if (adpModule) {
            for (auto &&module : adpModule->ModuleList()) {
                insertChild(row++, module);
            }
        }
    }
}

void BluetoothModule::updateWidget()
{
    setHidden(m_valueMap.isEmpty());
}
