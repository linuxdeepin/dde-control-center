// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothwidget.h"
#include "adapterwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/bluetooth/bluetoothmodel.h"
#include "modules/bluetooth/bluetoothworker.h"

#include <QVBoxLayout>
#include <QApplication>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;
using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;

BluetoothWidget::BluetoothWidget(BluetoothModel *model)
    : ContentWidget(nullptr)
    , m_model(model)
    , m_tFrame(new TranslucentFrame)
{
    setAccessibleName("BluetoothWidget");

    layout()->setContentsMargins(0, 10, 0, 10); //设置列表与背景上下间距为10
    setContent(m_tFrame);

    setObjectName("Bluetooth");
    m_bluetoothWorker = &dcc::bluetooth::BluetoothWorker::Instance();

    setModel(model);
    QTimer::singleShot(1, this, &BluetoothWidget::setVisibleState);
}

BluetoothWidget::~BluetoothWidget()
{
    for (const Adapter *adapter : m_model->adapters()) {
        m_bluetoothWorker->setAdapterDiscovering(QDBusObjectPath(adapter->id()), false);
    }
}

void BluetoothWidget::setModel(BluetoothModel *model)
{
    model->disconnect(this);
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemoved, this, &BluetoothWidget::removeAdapter);

    for (const Adapter *adapter : model->adapters())
        addAdapter(adapter);
}

void BluetoothWidget::loadDetailPage()
{
    if (m_valueMap.count() == 0) {
        return;
    }
    m_valueMap.begin().value()->loadDetailPage();
}

AdapterWidget *BluetoothWidget::getAdapter(const Adapter *adapter)
{
    AdapterWidget *adpWidget = new AdapterWidget(adapter, m_model);

    const QDBusObjectPath path(adapter->id());

    connect(adpWidget, &AdapterWidget::requestSetToggleAdapter, this, &BluetoothWidget::requestSetToggleAdapter);
    connect(adpWidget, &AdapterWidget::notifyRemoveDevice, this, &BluetoothWidget::updateWidget);
    connect(adpWidget, &AdapterWidget::requestConnectDevice, this, &BluetoothWidget::requestConnectDevice);
    connect(adpWidget, &AdapterWidget::requestDisconnectDevice, this, &BluetoothWidget::requestDisconnectDevice);
    connect(adpWidget, &AdapterWidget::requestSetAlias, this, &BluetoothWidget::requestSetAlias);
    connect(adpWidget, &AdapterWidget::requestShowDetail, this, &BluetoothWidget::showDeviceDetail);
    connect(adpWidget, &AdapterWidget::notifyLoadFinished, this, &BluetoothWidget::updateWidget);
    connect(adpWidget, &AdapterWidget::requestRefresh, this, &BluetoothWidget::requestRefresh);
    connect(adpWidget, &AdapterWidget::requestDiscoverable, this, &BluetoothWidget::requestDiscoverable);
    connect(adpWidget, &AdapterWidget::requestDiscoverable, this, &BluetoothWidget::requestDiscoverable);
    connect(adpWidget, &AdapterWidget::requestSetDisplaySwitch, m_bluetoothWorker, &BluetoothWorker::setDisplaySwitch);

    m_bluetoothWorker->setAdapterDiscovering(path, true);
    m_valueMap[adapter] = adpWidget;
    return adpWidget;
}

void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        QLayout *layout = m_tFrame->layout();

        if (!layout) {
            layout = new QVBoxLayout;
        }

        layout->addWidget(getAdapter(adapter));
        m_tFrame->setLayout(layout);

        setVisibleState();
    }
}

void BluetoothWidget::removeAdapter(const Adapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        QWidget *w = m_valueMap.take(adapter);
        m_tFrame->layout()->removeWidget(w);
        w->setParent(nullptr);
        w->deleteLater();
        setVisibleState();
        updateWidget();
    }
}

void BluetoothWidget::updateWidget()
{
    auto lastFocusWidget = QApplication::focusWidget();
    QLayout *layout = m_tFrame->layout();
    //每次添加蓝牙设备都会重新使用一个新的QVBoxLayout进行布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    if (layout) {
        //将旧的蓝牙设备数据取出来放在新的QVBoxLayout中
        while (QLayoutItem *item = layout->takeAt(0)) {
            //若当前为nullptr,还需要继续判断后面的数据
            if (!item->widget()) {
                continue;
            }
            vLayout->addWidget(item->widget(), 0, Qt::AlignTop);
        }
        layout->setParent(nullptr);
        layout->deleteLater();
        layout = nullptr;
    }

    m_tFrame->deleteLater();
    m_tFrame = new TranslucentFrame;
    vLayout->addStretch();
    m_tFrame->setLayout(vLayout);
    setContent(m_tFrame);

    // 上面代码会导致焦点丢失，导致焦点跳转到其他控件
    if (lastFocusWidget) {
        lastFocusWidget->setFocus();
    }
}

void BluetoothWidget::setVisibleState()
{
    Q_EMIT requestModuleVisible(m_valueMap.size());
}

void BluetoothWidget::requestRefresh(const dcc::bluetooth::Adapter *adapter)
{
    m_bluetoothWorker->setAdapterDiscoverable(adapter->id());
}
