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
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/titlelabel.h"
#include "window/utils.h"

#include <DListView>

#include <QVBoxLayout>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QApplication>

using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DWIDGET_USE_NAMESPACE

AdapterWidget::AdapterWidget(const dcc::bluetooth::Adapter *adapter, dcc::bluetooth::BluetoothModel *model)
    : m_titleEdit(new TitleEdit)
    , m_adapter(adapter)
    , m_switch(new SwitchWidget(nullptr, m_titleEdit))
    , m_switchFlag(Finished)
    , m_isFirstSetPower(true)
    , m_model(model)
    , m_isNotFirst(false)
{
    //~ contents_path /bluetooth/My Devices
    m_myDevicesGroup = new TitleLabel(tr("My Devices"));
    m_myDevicesGroup->setVisible(false);

    //~ contents_path /bluetooth/Other Devices
    m_otherDevicesGroup = new TitleLabel(tr("Other Devices"));
    QHBoxLayout *pshlayout = new QHBoxLayout;
    m_spinnerBtn = new DSpinner(m_titleEdit);

    pshlayout->addWidget(m_switch);
    pshlayout->addWidget(m_spinnerBtn);
    m_spinnerBtn->setFixedSize(24, 24);
    m_spinnerBtn->start();
    m_spinnerBtn->hide();
    m_spinner = new DSpinner();
    m_spinner->setFixedSize(24, 24);
    m_spinner->start();
    m_spinner->setVisible(false);
    m_refreshBtn = new DIconButton(this);
    m_refreshBtn->setFixedSize(24, 24);
    m_refreshBtn->setIcon(QIcon::fromTheme("dcc_refresh"));
    QHBoxLayout *phlayout = new QHBoxLayout;
    phlayout->addWidget(m_otherDevicesGroup);
    phlayout->addWidget(m_spinner);
    phlayout->addWidget(m_refreshBtn);

    m_switch->addBackground();
    m_switch->layout()->setContentsMargins(10, 10, 10, 10);
    m_switch->setContentsMargins(0, 0, 10, 0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    m_tip = new QLabel(tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"));
    m_tip->setVisible(!m_switch->checked());
    m_tip->setWordWrap(true);
    m_tip->setContentsMargins(16, 0, 10, 0);

    m_myDeviceListView = new DListView(this);
    m_myDeviceModel = new QStandardItemModel(m_myDeviceListView);
    m_myDeviceListView->setAccessibleName("List_mydevicelist");
    m_myDeviceListView->setFrameShape(QFrame::NoFrame);
    m_myDeviceListView->setModel(m_myDeviceModel);
    m_myDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_myDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_myDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_myDeviceListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setViewportMargins(ScrollAreaMargins);
    m_myDeviceListView->setSelectionMode(QListView::SelectionMode::NoSelection);

    m_otherDeviceListView = new DListView(this);
    m_otherDeviceModel = new QStandardItemModel(m_otherDeviceListView);
    m_otherDeviceListView->setAccessibleName("List_otherdevicelist");
    m_otherDeviceListView->setFrameShape(QFrame::NoFrame);
    m_otherDeviceListView->setModel(m_otherDeviceModel);
    m_otherDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_otherDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_otherDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_otherDeviceListView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setViewportMargins(ScrollAreaMargins);
    m_otherDeviceListView->setSelectionMode(QListView::SelectionMode::NoSelection);

    layout->addSpacing(10);
    layout->addLayout(pshlayout);
    layout->addWidget(m_tip, 0, Qt::AlignTop);
    layout->addSpacing(10);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_myDeviceListView);
    layout->addSpacing(10);
    layout->addLayout(phlayout);
    layout->addWidget(m_otherDeviceListView);
    layout->addSpacing(interval);
    layout->addStretch();
    layout->setContentsMargins(0,0,15,0);

    connect(m_switch, &SwitchWidget::checkedChanged, this, [ = ] (const bool &value) {
        if (!m_isNotFirst) {
            m_dtime.start();
            m_isNotFirst = true;
            toggleSwitch(value);
        } else {
            if (m_dtime.elapsed() < 300) {
                m_switch->blockSignals(true);
                m_switch->setChecked(!value);
                m_switch->blockSignals(false);
            } else {
                toggleSwitch(value);
            }
        }
        m_dtime.restart();
    });

    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, this, [ = ](const QString &alias) {
        Q_EMIT requestSetAlias(adapter, alias);
    });

    connect(m_myDeviceListView, &DListView::clicked, this, [this](const QModelIndex &idx) {
        m_otherDeviceListView->clearSelection();
        const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel) {
            return;
        }
        DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(idx.row()));
        if (!item) {
            return;
        }
        for (auto it : m_myDevices) {
            if (it != NULL && it->getStandardItem() == item) {
                if (it->device()->state() != Device::StateConnected)
                    it->requestConnectDevice(it->device());

                Q_EMIT requestShowDetail(m_adapter, it->device());
                break;
            }
        }
    });
    connect(m_myDeviceListView, &DListView::activated, m_myDeviceListView, &DListView::clicked);

    connect(m_otherDeviceListView, &DListView::clicked, this, [this](const QModelIndex & idx) {
        m_myDeviceListView->clearSelection();
        const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel) {
            return;
        }
        DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(idx.row()));
        if (!item) {
            return;
        }
        for (auto it : m_deviceLists) {
            if (it != NULL && it->getStandardItem() == item)
                it->requestConnectDevice(it->device());
        }
    });
    connect(m_otherDeviceListView, &DListView::activated, m_otherDeviceListView, &DListView::clicked);

    connect(m_refreshBtn, &DIconButton::clicked, this , [=]{
        Q_EMIT requestRefresh(m_adapter);
    });

    setLayout(layout);
    QTimer::singleShot(0, this, [this] {
        setAdapter(m_adapter);
    });
}

AdapterWidget::~AdapterWidget()
{
    //手动删除qlist中的指针
    for (auto it : m_myDevices) {
        if (it != NULL){
            delete it;
            it = NULL;
        }
    }

    for (auto it : m_deviceLists) {
        if (it != NULL){
            delete it;
            it = NULL;
        }
    }

    m_myDevices.clear();
    m_deviceLists.clear();
}

bool AdapterWidget::getSwitchState()
{
    return m_switch ? m_switch->checked() : false;
}

void AdapterWidget::loadDetailPage()
{
    if (m_myDevices.count() == 0 || m_myDevices.at(0) == NULL) {
        return;
    }

    Q_EMIT requestShowDetail(m_adapter, m_myDevices.at(0)->device());
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_titleEdit, &TitleEdit::setTitle, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice, Qt::QueuedConnection);
    connect(adapter, &Adapter::poweredChanged, this, &AdapterWidget::onPowerStatus, Qt::QueuedConnection);
    connect(m_model, &BluetoothModel::adpaterPowerd, m_switch, [ = ] {
        m_switch->switchButton()->show();
        m_spinnerBtn->hide();
    });
    connect(m_model, &BluetoothModel::loadStatus, m_switch, [ = ] {
        m_switch->switchButton()->hide();
        m_spinnerBtn->show();
    });

    m_titleEdit->setTitle(adapter->name());
    for (const Device *device : adapter->devices()) {
        addDevice(device);
    }
    onPowerStatus(adapter->powered(), adapter->discovering());
}

void AdapterWidget::onPowerStatus(bool bPower, bool bDiscovering)
{
    //bDiscovering说明设备正在搜索设备，蓝牙状态发生了改变
    //如果蓝牙在打开状态则如果bDiscoersing为true时候证明蓝牙打开手开始工作
    if (m_switchFlag == OnPower && bPower && bDiscovering) {
        m_switch->setEnabled(true);
        m_switchFlag = Finished;
    }

    //如果蓝牙在关闭状态则如果bDiscoersing为false时候证明蓝牙已经关闭
    if (m_switchFlag == OffPower && !bPower && !bDiscovering) {
        m_switch->setEnabled(true);
        m_switchFlag = Finished;
    }

    //第一次设置m_switch状态
    if (m_isFirstSetPower) {
        m_isFirstSetPower = false;
        m_switch->setEnabled(true);
        m_switchFlag = Finished;
    }

    m_switch->setChecked(bPower);
    m_tip->setVisible(!bPower);
    m_myDevicesGroup->setVisible(bPower && !m_myDevices.isEmpty());
    m_otherDevicesGroup->setVisible(bPower);
    m_spinner->setVisible(bPower && bDiscovering);
    m_refreshBtn->setVisible(bPower && !bDiscovering);
    m_myDeviceListView->setVisible(bPower && !m_myDevices.isEmpty());
    m_otherDeviceListView->setVisible(bPower);
    Q_EMIT notifyLoadFinished();
}

void AdapterWidget::toggleSwitch(const bool checked)
{
    QApplication::focusWidget()->clearFocus();
    Q_ASSERT(m_switchFlag == Finished);
    //开始关闭或者开启时候switchbutton不可被点击
    m_switch->setEnabled(false);

    //记录当前开关状态
    if (checked) {
        m_switchFlag = OnPower;
    } else {
        m_switchFlag = OffPower;

        onPowerStatus(false, true);

        for (auto it : m_myDevices) {
            if (it->device()->connecting()) {
                Q_EMIT requestDisconnectDevice(it->device());
            }
        }
    }

    onPowerStatus(false, true);

    Q_EMIT requestSetToggleAdapter(m_adapter, checked);
}

void AdapterWidget::categoryDevice(DeviceSettingsItem *deviceItem, const bool paired)
{
    if (paired) {
        DStandardItem *dListItem = deviceItem->getStandardItem(m_myDeviceListView);
        m_myDevices << deviceItem;
        m_myDeviceModel->appendRow(dListItem);
    } else {
        DStandardItem *dListItem = deviceItem->getStandardItem(m_otherDeviceListView);
        m_otherDeviceModel->appendRow(dListItem);
    }
    bool isVisible = !m_myDevices.isEmpty() && m_switch->checked();
    m_myDevicesGroup->setVisible(isVisible);
    m_myDeviceListView->setVisible(isVisible);
}

void AdapterWidget::addDevice(const Device *device)
{
    qDebug() << "addDevice: " << device->name();
    QPointer<DeviceSettingsItem> deviceItem = new DeviceSettingsItem(device, style());
    categoryDevice(deviceItem, device->paired());

    connect(deviceItem, &DeviceSettingsItem::requestConnectDevice, this, &AdapterWidget::requestConnectDevice);
    connect(device, &Device::pairedChanged, this, [this, deviceItem](const bool paired) {
        if (paired) {
            qDebug() << "paired :" << deviceItem->device()->name();
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
            m_otherDeviceModel->removeRow(otherDeviceIndex.row());
            DStandardItem *dListItem = deviceItem->createStandardItem(m_myDeviceListView);
            m_myDevices << deviceItem;
            m_myDeviceModel->appendRow(dListItem);
        } else {
            qDebug() << "unpaired :" << deviceItem->device()->name();
            for (auto it : m_myDevices) {
                if (it != NULL && it == deviceItem) {
                    m_myDevices.removeOne(it);
                    break;
                }
            }
            DStandardItem *item = deviceItem->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            DStandardItem *dListItem = deviceItem->createStandardItem(m_otherDeviceListView);
            m_otherDeviceModel->appendRow(dListItem);
        }
        bool isVisible = !m_myDevices.isEmpty() && m_switch->checked();
        m_myDevicesGroup->setVisible(isVisible);
        m_myDeviceListView->setVisible(isVisible);
    });
    connect(deviceItem, &DeviceSettingsItem::requestShowDetail, this, [this](const Device * device) {
        Q_EMIT requestShowDetail(m_adapter, device);
    });

    m_deviceLists << deviceItem;
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    bool isFind = false;
    for (auto it : m_myDevices) {
        if (it != NULL && it->device() != nullptr && it->device()->id() == deviceId) {

            qDebug() << "removeDevice my: " << it->device()->name();
            DStandardItem *item = it->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            m_myDevices.removeOne(it);
            m_deviceLists.removeOne(it);

            delete it;
            it = NULL;

            Q_EMIT notifyRemoveDevice();
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        for (auto it : m_deviceLists) {
            if (it != NULL && it->device() != nullptr && it->device()->id() == deviceId) {

                qDebug() << "removeDevice other: " << it->device()->name();
                DStandardItem *item = it->getStandardItem();
                QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
                m_otherDeviceModel->removeRow(otherDeviceIndex.row());

                m_deviceLists.removeOne(it);

                delete it;
                it = NULL;

                Q_EMIT notifyRemoveDevice();
                break;
            }
        }

    }

    if (m_myDevices.isEmpty()) {
        m_myDevicesGroup->hide();
        m_myDeviceListView->hide();
    }
}

const Adapter *AdapterWidget::adapter() const
{
    return m_adapter;
}
