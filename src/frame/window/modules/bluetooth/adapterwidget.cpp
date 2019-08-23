/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "adapterwidget.h"
#include "titleedit.h"
#include "devicesettingsitem.h"
#include "modules/bluetooth/adapter.h"
#include "modules/bluetooth/detailpage.h"
#include "modules/bluetooth/adapter.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/loadingindicator.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

#include <DListView>

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>

using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DWIDGET_USE_NAMESPACE

AdapterWidget::AdapterWidget(const dcc::bluetooth::Adapter *adapter)
    : m_adapter(adapter)
    , m_titleEdit(new TitleEdit)
    , m_switch(new SwitchWidget(m_titleEdit))
    , m_titleGroup(new SettingsGroup)
    , m_myDeviceLabel(new QLabel(tr("My devices")))
    , m_otherDeviceLabel(new QLabel(tr("Other devices")))
{
    m_switch->setFixedHeight(36);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    m_titleGroup->appendItem(m_switch);

    m_tip = new QLabel(tr("Enable bluetooth to find nearby devices (loudspeaker, keyboard, mouse)"));
    m_tip->setVisible(!m_switch->checked());
    m_tip->setWordWrap(true);
    m_tip->setContentsMargins(16, 0, 10, 0);

    m_myDeviceListView = new DListView(this);
    m_myDeviceModel = new QStandardItemModel(m_myDeviceListView);
    m_myDeviceListView->setFrameShape(QFrame::NoFrame);
    m_myDeviceListView->setModel(m_myDeviceModel);
    m_myDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_myDeviceListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_otherDeviceListView = new DListView(this);
    m_otherDeviceModel = new QStandardItemModel(m_otherDeviceListView);
    m_otherDeviceListView->setFrameShape(QFrame::NoFrame);
    m_otherDeviceListView->setModel(m_otherDeviceModel);
    m_otherDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_otherDeviceListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addSpacing(10);
    layout->addWidget(m_titleGroup);
    layout->addWidget(m_tip, 0, Qt::AlignTop);
    layout->addSpacing(10);
    layout->addWidget(m_myDeviceLabel);
    layout->addWidget(m_myDeviceListView);
    layout->addSpacing(10);
    layout->addWidget(m_otherDeviceLabel);
    layout->addWidget(m_otherDeviceListView);
    layout->addStretch();

    connect(m_switch, &SwitchWidget::checkedChanged, this, &AdapterWidget::toggleSwitch);
    connect(m_switch, &SwitchWidget::checkedChanged, [ = ](bool state) {
        m_tip->setVisible(!state);
    });
    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, [ = ](const QString & alias) {
        Q_EMIT requestSetAlias(adapter, alias);
    });

    setLayout(layout);
    setAdapter(adapter);
}

void AdapterWidget::updateHeight()
{
    if (m_myDeviceListView) {
        QTimer::singleShot(0, this, [this] {
            // qDebug() << "my height: " << m_myDeviceListView->contentsSize().height();
            m_myDeviceListView->setFixedHeight(m_myDeviceListView->contentsSize().height());
        });
    }
    if (m_otherDeviceListView) {
        QTimer::singleShot(0, this, [this] {
            // qDebug() << "other height: " << m_otherDeviceListView->contentsSize();
            m_otherDeviceListView->setFixedHeight(m_otherDeviceListView->contentsSize().height());
        });
    }
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_titleEdit, &TitleEdit::setTitle, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::poweredChanged, m_switch, &SwitchWidget::setChecked, Qt::QueuedConnection);

    m_switch->blockSignals(true);
    m_titleEdit->setTitle(adapter->name());
    m_switch->setChecked(adapter->powered());
    m_tip->setVisible(!m_switch->checked());
    m_switch->blockSignals(false);

    blockSignals(true);
    toggleSwitch(adapter->powered());
    blockSignals(false);

    for (const Device *device : adapter->devices()) {
        addDevice(device);
    }
}

void AdapterWidget::toggleSwitch(const bool &checked)
{
    m_myDeviceLabel->setVisible(checked && !m_myDevices.isEmpty());
    m_otherDeviceLabel->setVisible(checked);
    m_myDeviceListView->setVisible(checked && !m_myDevices.isEmpty());
    m_otherDeviceListView->setVisible(checked);

    Q_EMIT requestSetToggleAdapter(m_adapter, checked);
}

void AdapterWidget::addDevice(const Device *device)
{
    DeviceSettingsItem *deviceItem = new DeviceSettingsItem(device, style());
    auto CategoryDevice = [this, deviceItem](const bool paired) {
        if (paired) {
            DStandardItem *dListItem = deviceItem->getStandardItem();
            m_myDevices << deviceItem;
            m_myDeviceModel->appendRow(dListItem);
        } else {
            DStandardItem *dListItem = deviceItem->getStandardItem();
            m_otherDeviceModel->appendRow(dListItem);
        }
        updateHeight();
        m_myDeviceListView->setVisible(!m_myDevices.isEmpty() && m_switch->checked());
    };
    CategoryDevice(device->paired());

    connect(deviceItem, &DeviceSettingsItem::requestConnectDevice, this, &AdapterWidget::requestConnectDevice);
    connect(device, &Device::pairedChanged, [this, deviceItem](const bool paired) {
        if (paired) {
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
            m_otherDeviceModel->removeRow(otherDeviceIndex.row());
            DStandardItem *dListItem = deviceItem->createStandardItem();
            m_myDevices << deviceItem;
            m_myDeviceModel->appendRow(dListItem);
        } else {
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            DStandardItem *dListItem = deviceItem->createStandardItem();
            m_otherDeviceModel->appendRow(dListItem);
        }
        updateHeight();
        m_myDeviceListView->setVisible(!m_myDevices.isEmpty() && m_switch->checked());
    });
    connect(deviceItem, &DeviceSettingsItem::requestShowDetail, [this](const Device *device) {
        Q_EMIT requestShowDetail(m_adapter, device);
    });

    m_deviceLists << deviceItem;
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    bool isFind = false;
    for (auto it = m_myDevices.begin(); it != m_myDevices.end(); ++it) {
        if ((*it)->device()->id() == deviceId) {
            DStandardItem *item = (*it)->getStandardItem();
            m_myDevices.removeOne(*it);
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        for (auto it = m_deviceLists.begin(); it != m_deviceLists.end(); ++it) {
            if ((*it)->device()->id() == deviceId) {
                DStandardItem *item = (*it)->getStandardItem();
                QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
                m_otherDeviceModel->removeRow(otherDeviceIndex.row());
                m_deviceLists.erase(it);
                break;
            }
        }
    }
    updateHeight();
    if (m_myDevices.isEmpty()) {
        m_myDeviceListView->hide();
    }
}

const Adapter *AdapterWidget::adapter() const
{
    return m_adapter;
}
