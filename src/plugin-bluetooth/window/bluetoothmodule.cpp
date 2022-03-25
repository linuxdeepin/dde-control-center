/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "bluetoothmodule.h"
#include "bluetoothmodel.h"
#include "adaptermodule.h"
#include "bluetoothworker.h"
#include "bluetoothadapter.h"
#include "bluetoothdevice.h"
#include "pincodedialog.h"

#include <QDBusObjectPath>
#include <QApplication>

DCC_USE_NAMESPACE

BluetoothModule::BluetoothModule(QObject *parent)
    : ModuleObject("bluetooth", tr("Bluetooth"), tr("Bluetooth"), QIcon::fromTheme("dcc_nav_bluetooth"), parent)
    , index(0)
    , m_parent(nullptr)
{
    setChildType(ModuleObject::ChildType::Page);

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
}

void BluetoothModule::deactive()
{
    for (auto it : m_valueMap) {
        it->deactive();
    }
}

bool BluetoothModule::hasDevice()
{
    return !m_valueMap.isEmpty();
}

void BluetoothModule::showPinCode(const QDBusObjectPath &device, const QString &code)
{
    qDebug() << "request confirmation: " << device.path() << code;
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
    connect(adpWidget, &AdapterModule::requestSetDevAlias, m_work, &BluetoothWorker::setDeviceAlias);
    connect(adpWidget, &AdapterModule::requestRefresh, this, &BluetoothModule::requestRefresh);
    connect(adpWidget, &AdapterModule::requestDiscoverable, this, &BluetoothModule::requestDiscoverable);
    connect(adpWidget, &AdapterModule::requestDiscoverable, this, &BluetoothModule::requestDiscoverable);
    connect(adpWidget, &AdapterModule::requestSetDisplaySwitch, m_work, &BluetoothWorker::setDisplaySwitch);
    connect(adpWidget, &AdapterModule::requestIgnoreDevice, m_work, &BluetoothWorker::ignoreDevice);

    m_work->setAdapterDiscovering(path, true);
    m_valueMap[adapter] = adpWidget;
    return adpWidget;
}

void BluetoothModule::addAdapter(const BluetoothAdapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        AdapterModule *adpModule = getAdapter(adapter);
        const QList<DCC_NAMESPACE::ModuleObject *> &list = adpModule->ModuleList();
        for (DCC_NAMESPACE::ModuleObject *obj : list) {
            appendChild(obj);
        }
        setVisibleState();
        updateWidget();
    }
}
void BluetoothModule::removeAdapter(const BluetoothAdapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        AdapterModule *adpModule = m_valueMap.take(adapter);
        const QList<DCC_NAMESPACE::ModuleObject *> &list = adpModule->ModuleList();
        for (DCC_NAMESPACE::ModuleObject *obj : list) {
            removeChild(obj);
        }
        adpModule->setParent(nullptr);
        adpModule->deleteLater();
        setVisibleState();
        updateWidget();
    }
}

void BluetoothModule::requestRefresh(const BluetoothAdapter *adapter)
{
    m_work->setAdapterDiscoverable(adapter->id());
}

void BluetoothModule::updateWidget()
{
    if (!m_parent)
        return;
    if (m_valueMap.isEmpty()) {
        if (m_parent->findChild(this) != -1) {
            index = m_parent->childrens().indexOf(this);
            m_parent->removeChild(this);
        }
    } else {
        if (m_parent->findChild(this) == -1) {
            m_parent->insertChild(index, this);
        }
    }
}

bool BluetoothModule::event(QEvent *ev)
{
    if (ev->type() == QEvent::ThreadChange) {
        m_parent = qobject_cast<ModuleObject *>(parent());
        updateWidget();
    }
    return ModuleObject::event(ev);
}

void BluetoothModule::setVisibleState()
{
    Q_EMIT requestModuleVisible(m_valueMap.size());
}
