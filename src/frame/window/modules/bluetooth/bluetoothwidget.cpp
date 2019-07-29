/*
 * Copyright (C) 2016 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang<andywang_cm@deepin.com>
 *
 * Maintainer: andywang<andywang_cm@deepin.com>
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

#include "bluetoothwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/bluetooth/bluetoothmodel.h"
#include "modules/bluetooth/bluetoothworker.h"
#include "adapterwidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;
using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;

BluetoothWidget::BluetoothWidget(BluetoothModel *model) : m_model(model)
{
    setObjectName("Bluetooth");
    m_bluetoothWorker = &dcc::bluetooth::BluetoothWorker::Instance();
    m_contentLayout = new QVBoxLayout;

    TranslucentFrame *tFrame = new TranslucentFrame;
    tFrame->setLayout(m_contentLayout);
    setContent(tFrame);

    setModel(model);
    QTimer::singleShot(1, this, &BluetoothWidget::setVisibleState);
}

void BluetoothWidget::setModel(BluetoothModel *model)
{
    model->disconnect(this);
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemoved, this, &BluetoothWidget::removeAdapter);

    for (const Adapter *adapter : model->adapters())
        addAdapter(adapter);
}

AdapterWidget *BluetoothWidget::getAdapter(const Adapter *adapter)
{
    AdapterWidget *adpWidget = new AdapterWidget(adapter);

    const QDBusObjectPath path(adapter->id());

    connect(adpWidget, &AdapterWidget::requestSetToggleAdapter, this, &BluetoothWidget::requestSetToggleAdapter);
    connect(adpWidget, &AdapterWidget::requestConnectDevice, this, &BluetoothWidget::requestConnectDevice);
    connect(adpWidget, &AdapterWidget::requestSetAlias, this, &BluetoothWidget::requestSetAlias);
    connect(adpWidget, &AdapterWidget::requestShowDetail, this, &BluetoothWidget::showDeviceDetail);

    m_bluetoothWorker->setAdapterDiscovering(path, true);

    m_valueMap[adapter] = adpWidget;
    return adpWidget;
}

void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        setVisibleState();
        AdapterWidget *adapterWidget = getAdapter(adapter);
        m_contentLayout->addWidget(adapterWidget);
    }
}

void BluetoothWidget::removeAdapter(const Adapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        QWidget *w = m_valueMap.take(adapter);
        m_contentLayout->removeWidget(w);
        w->deleteLater();
        setVisibleState();
    }
}

void BluetoothWidget::setVisibleState()
{
    Q_EMIT requestModuleVisible(m_valueMap.size());
}
