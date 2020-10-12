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
#include <QWidget>
#include <QApplication>

using BluetoothInter = com::deepin::daemon::Bluetooth;

using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DWIDGET_USE_NAMESPACE

AdapterWidget::AdapterWidget(const dcc::bluetooth::Adapter *adapter, dcc::bluetooth::BluetoothModel *model)
    : m_titleEdit(new TitleEdit)
    , m_hideAnonymousLabel(new QLabel(tr("Show Bluetooth devices without names")))
    , m_adapter(adapter)
    , m_powerSwitch(new SwitchWidget(nullptr, m_titleEdit))
    , m_showAnonymousCheckBox(new DCheckBox(this))
    , m_model(model)
    , m_discoverySwitch(new SwitchWidget(tr("Allow other Bluetooth devices to find this device")))
    , m_lastPowerCheck(false)
    , m_delaySortTimer(new QTimer(this))
    , m_bluetoothInter("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)
{
    initMember();
    initUI();
    initConnect();

    QTimer::singleShot(0, this, [this] {
        setAdapter(m_adapter);
    });
}

AdapterWidget::~AdapterWidget()
{
    qDeleteAll(m_deviceLists);
    m_myDevices.clear();
    m_deviceLists.clear();
}

bool AdapterWidget::getSwitchState()
{
    return m_powerSwitch ? m_powerSwitch->checked() : false;
}

void AdapterWidget::initMember()
{
    m_delaySortTimer->setSingleShot(true);
    m_delaySortTimer->setInterval(10);

    m_showUnnamedDevices = m_bluetoothInter.displaySwitch();
}

void AdapterWidget::initUI()
{
    //~ contents_path /bluetooth/My Devices
    m_myDevicesGroup = new TitleLabel(tr("My Devices"));
    m_myDevicesGroup->setVisible(false);

    //~ contents_path /bluetooth/Other Devices
    m_otherDevicesGroup = new TitleLabel(tr("Other Devices"));
    m_spinnerBtn = new DSpinner(m_titleEdit);

    m_titleEdit->setMinimumWidth(10);
    //把动画按钮放在蓝牙开关前面
    m_powerSwitch->getMainLayout()->insertWidget(1, m_spinnerBtn, Qt::AlignVCenter);

    m_spinnerBtn->setFixedSize(24, 24);
    m_spinnerBtn->start();
    m_spinnerBtn->hide();
    m_spinner = new DSpinner();
    m_spinner->setFixedSize(24, 24);
    m_spinner->start();
    m_refreshBtn = new DIconButton(this);
    m_refreshBtn->setFixedSize(36, 36);
    m_refreshBtn->setIcon(QIcon::fromTheme("dcc_refresh"));
    m_showAnonymousCheckBox->setChecked(m_showUnnamedDevices);
    m_showAnonymousCheckBox->setAccessibleName("Btn_anonymouscheckbox");

    m_hideAnonymousLabel->setFixedHeight(36);
    m_hideAnonymousLabel->setMinimumWidth(10);
    m_showAnonymousCheckBox->setFixedHeight(36);
    QHBoxLayout *phlayoutShowAnonymous = new QHBoxLayout;
    phlayoutShowAnonymous->addWidget(m_showAnonymousCheckBox);
    phlayoutShowAnonymous->addWidget(m_hideAnonymousLabel);
    phlayoutShowAnonymous->addStretch();
    phlayoutShowAnonymous->addWidget(m_spinner);
    phlayoutShowAnonymous->addWidget(m_refreshBtn);

    m_powerSwitch->setFixedHeight(36);
    m_powerSwitch->setContentsMargins(0, 0, 0, 0);
    m_discoverySwitch->leftWidget()->setMinimumWidth(10);
    m_discoverySwitch->setContentsMargins(0, 0, 0, 0);
    m_discoverySwitch->setFixedHeight(36);

    SettingsGroup *settingsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    settingsGrp->setContentsMargins(0, 0, 0, 0);
    settingsGrp->layout()->setMargin(0);
    settingsGrp->setSpacing(1);

    settingsGrp->appendItem(m_powerSwitch);
    settingsGrp->appendItem(m_discoverySwitch);

    m_tip = new QLabel(tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"));
    m_tip->setVisible(!m_powerSwitch->checked());
    m_tip->setWordWrap(true);
    m_tip->setContentsMargins(16, 0, 10, 0);

    m_myDeviceListView = new DListView(this);
    m_myDeviceModel = new QStandardItemModel(m_myDeviceListView);
    m_myDeviceModel->setSortRole(BtStandardItem::SortRole);
    m_myDeviceListView->setAccessibleName("List_mydevicelist");
    m_myDeviceListView->setFrameShape(QFrame::NoFrame);
    m_myDeviceListView->setModel(m_myDeviceModel);
    m_myDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_myDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_myDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_myDeviceListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_myDeviceListView->setViewportMargins(0, 0, 0, 0);

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
    m_otherDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_otherDeviceListView->setViewportMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    layout->addSpacing(10);
    layout->addWidget(settingsGrp);
    layout->addWidget(m_tip, 0, Qt::AlignTop);
    layout->addSpacing(10);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_myDeviceListView);
    layout->addSpacing(10);

    layout->addWidget(m_otherDevicesGroup);
    layout->addLayout(phlayoutShowAnonymous);
    layout->addWidget(m_otherDeviceListView);
    layout->addSpacing(interval);
    layout->addStretch();
    layout->setContentsMargins(ThirdPageContentsMargins);

    setLayout(layout);
}

void AdapterWidget::initConnect()
{
    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, this, [ = ](const QString & alias) {
        Q_EMIT requestSetAlias(m_adapter, alias);
    });

    connect(m_myDeviceListView, &DListView::clicked, this, [this](const QModelIndex & idx) {
        m_otherDeviceListView->clearSelection();
        const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel) {
            return;
        }
        BtStandardItem *item = dynamic_cast<BtStandardItem *>(deviceModel->item(idx.row()));
        if (!item) {
            return;
        }
        for (auto it : m_myDevices) {
            if (it && it->device() && it->getStandardItem() == item) {
                if (it->device()->state() != Device::StateConnected) {
                    it->requestConnectDevice(it->device(), m_adapter);
                }
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
        BtStandardItem *item = dynamic_cast<BtStandardItem *>(deviceModel->item(idx.row()));
        if (!item) {
            return;
        }
        for (auto it : m_deviceLists) {
            if (it && it->device() && it->getStandardItem() == item) {
                it->requestConnectDevice(it->device(), m_adapter);
                BtSortInfo info = it->getStandardItem()->sortInfo();
                info.time = static_cast<int>(QDateTime::currentDateTime().toTime_t());
                it->getStandardItem()->setSortInfo(info);
                m_delaySortTimer->start();
                break;
            }
        }
    });

    connect(m_otherDeviceListView, &DListView::activated, m_otherDeviceListView, &DListView::clicked);

    connect(m_refreshBtn, &DIconButton::clicked, this, [ = ] {
        Q_EMIT requestRefresh(m_adapter);
    });

    connect(m_powerSwitch, &SwitchWidget::checkedChanged, this, [ = ](const bool check) {
        //关闭蓝牙的时候,直接隐藏列表
        if (!check) {
            onPowerStatus(false, false);
            if (m_adapter) {
                Q_EMIT m_adapter->closeDetailPage();
            }
        }
        m_powerSwitch->switchButton()->setEnabled(false);
        Q_EMIT requestSetToggleAdapter(m_adapter, check);
    });

    connect(m_discoverySwitch, &SwitchWidget::checkedChanged, this, &AdapterWidget::toggleDiscoverableSwitch);

    connect(&m_bluetoothInter, &BluetoothInter::DisplaySwitchChanged, m_showAnonymousCheckBox, &DCheckBox::setChecked);
    connect(m_showAnonymousCheckBox, &DCheckBox::stateChanged, this, [ = ](int state) {
        if (state == Qt::CheckState::Unchecked) {
            if (m_bluetoothInter.displaySwitch()) {
                m_bluetoothInter.setDisplaySwitch(false);
            }
            // 将蓝牙名称为空的设备过滤掉
            for (int i = 0; i < m_deviceLists.size(); i++) {
                DeviceSettingsItem *pDeviceItem = m_deviceLists[i];

                if (pDeviceItem->device()->paired())
                    continue;

                BtStandardItem *dListItem = pDeviceItem->getStandardItem();
                QModelIndex index = m_otherDeviceModel->indexFromItem(dListItem);
                if (index.isValid() && pDeviceItem->device()->name().isEmpty()) {
                    m_otherDeviceModel->takeRow(index.row());
                    m_delaySortTimer->start();
                }
            }
        } else {
            if (!m_bluetoothInter.displaySwitch()) {
                m_bluetoothInter.setDisplaySwitch(true);
            }
            // 显示所有蓝牙设备
            for (int i = 0; i < m_deviceLists.size(); i++) {
                DeviceSettingsItem *pDeviceItem = m_deviceLists[i];
                if (pDeviceItem->device()->paired())
                    continue;

                BtStandardItem *dListItem = pDeviceItem->getStandardItem();
                QModelIndex index = m_otherDeviceModel->indexFromItem(dListItem);
                if ((false == index.isValid()) && pDeviceItem->device()->name().isEmpty()) {
                    m_otherDeviceModel->insertRow(0, dListItem);
                    m_delaySortTimer->start();
                }
            }
        }
    });

    connect(m_delaySortTimer, &QTimer::timeout, this, [ = ] {
        m_myDeviceModel->sort(0, Qt::SortOrder::DescendingOrder);
        m_otherDeviceModel->sort(0, Qt::SortOrder::DescendingOrder);
    });
}

void AdapterWidget::loadDetailPage()
{
    if (m_myDevices.count() == 0) {
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
    connect(m_model, &BluetoothModel::adpaterPowerChanged, m_powerSwitch, [ = ] {
        m_powerSwitch->switchButton()->setEnabled(true);
        m_spinnerBtn->hide();
    });
    connect(adapter, &Adapter::loadStatus, m_powerSwitch, [ = ] {
        m_powerSwitch->switchButton()->setEnabled(false);
        m_spinnerBtn->show();
    });

    m_titleEdit->setTitle(adapter->name());
    for (QString id : adapter->devicesId()) {
        if (adapter->devices().contains(id)) {
            const Device *device = adapter->devices()[id];
            addDevice(device);
        }
    }
    connect(adapter, &Adapter::discoverableChanged, m_discoverySwitch, [ = ] {
        m_discoverySwitch->setChecked(adapter->discoverabled());
    });
    m_lastPowerCheck = adapter->powered();
    m_discoverySwitch->setChecked(m_adapter->discoverabled());
    onPowerStatus(adapter->powered(), adapter->discovering());
}

void AdapterWidget::onPowerStatus(bool bPower, bool bDiscovering)
{
    m_powerSwitch->setEnabled(true);
    m_powerSwitch->setChecked(bPower);
    m_discoverySwitch->setEnabled(true);
    m_discoverySwitch->setVisible(bPower);
    m_tip->setVisible(!bPower);
    m_discoverySwitch->setVisible(bPower);
    m_myDevicesGroup->setVisible(bPower && !m_myDevices.isEmpty());
    m_otherDevicesGroup->setVisible(bPower);
    m_showAnonymousCheckBox->setVisible(bPower);
    m_hideAnonymousLabel->setVisible(bPower);
    m_spinner->setVisible(bPower && bDiscovering);
    m_refreshBtn->setVisible(bPower && !bDiscovering);
    m_myDeviceListView->setVisible(bPower && !m_myDevices.isEmpty());
    m_otherDeviceListView->setVisible(bPower);
    Q_EMIT notifyLoadFinished();
}

void AdapterWidget::toggleSwitch(const bool checked)
{
    if (QApplication::focusWidget()) {
        QApplication::focusWidget()->clearFocus();
    }
    if (!checked) {
        for (auto it : m_myDevices) {
            if (it && it->device() && it->device()->connecting()) {
                Q_EMIT requestDisconnectDevice(it->device());
            }
        }
    }
    m_lastPowerCheck = checked;
    Q_EMIT requestSetToggleAdapter(m_adapter, checked);
}

void AdapterWidget::toggleDiscoverableSwitch(const bool checked)
{
    Q_EMIT requestDiscoverable(m_adapter, checked);
}

void AdapterWidget::categoryDevice(DeviceSettingsItem *deviceItem, const bool paired)
{
    if (paired) {
        BtStandardItem *dListItem = deviceItem->getStandardItem(m_myDeviceListView);
        m_myDevices << deviceItem;
        m_myDeviceModel->appendRow(dListItem);
        m_delaySortTimer->start();
    } else {
        BtStandardItem *dListItem = deviceItem->getStandardItem(m_otherDeviceListView);
        if (m_showAnonymousCheckBox->checkState() == Qt::CheckState::Unchecked) {
            if (false == deviceItem->device()->name().isEmpty()) { // 只关注有名称的蓝牙设备,没有名称的忽略
                m_otherDeviceModel->insertRow(0, dListItem);
                m_delaySortTimer->start();
            }
        } else {
            m_otherDeviceModel->insertRow(0, dListItem);
        }
    }
    bool isVisible = !m_myDevices.isEmpty() && m_powerSwitch->checked();
    m_myDevicesGroup->setVisible(isVisible);
    m_myDeviceListView->setVisible(isVisible);

    connect(deviceItem, &DeviceSettingsItem::requestSort, this, [ = ] {
        m_delaySortTimer->start();
    });
}

void AdapterWidget::addDevice(const Device *device)
{
    QPointer<DeviceSettingsItem> deviceItem = new DeviceSettingsItem(device, style());
    categoryDevice(deviceItem, device->paired());

    connect(deviceItem, &DeviceSettingsItem::requestConnectDevice, this, &AdapterWidget::requestConnectDevice);
    connect(device, &Device::pairedChanged, this, [this, deviceItem](const bool paired) {
        if (paired) {
            qDebug() << "paired :" << deviceItem->device()->name();
            BtStandardItem *item = deviceItem->getStandardItem();
            QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
            m_otherDeviceModel->removeRow(otherDeviceIndex.row());
            deviceItem->resetDeviceItem();
            BtStandardItem *dListItem = deviceItem->createStandardItem(m_myDeviceListView);
            m_myDevices << deviceItem;
            connect(deviceItem, &DeviceSettingsItem::requestSort, this, [ = ] {
                m_delaySortTimer->start();
            });
            m_myDeviceModel->appendRow(dListItem);
            m_delaySortTimer->start();
        } else {
            qDebug() << "unpaired :" << deviceItem->device()->name();
            for (auto it : m_myDevices) {
                if (it && it->device() && it == deviceItem) {
                    m_myDevices.removeOne(it);
                    break;
                }
            }
            BtStandardItem *item = deviceItem->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            BtStandardItem *dListItem = deviceItem->createStandardItem(m_otherDeviceListView);
            m_otherDeviceModel->appendRow(dListItem);
            m_delaySortTimer->start();
        }
        bool isVisible = !m_myDevices.isEmpty() && m_powerSwitch->checked();
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
        if (it && it->device() && it->device()->id() == deviceId) {
            BtStandardItem *item = it->getStandardItem();
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            m_myDeviceModel->removeRow(myDeviceIndex.row());
            m_myDevices.removeOne(it);
            m_deviceLists.removeOne(it);
            delete it;
            it = nullptr;
            Q_EMIT notifyRemoveDevice();
            isFind = true;
            break;
        }
    }
    if (!isFind) {
        for (auto it : m_deviceLists) {
            if (it && it->device() && it->device()->id() == deviceId) {
                BtStandardItem *item = it->getStandardItem();
                QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
                m_otherDeviceModel->removeRow(otherDeviceIndex.row());
                m_deviceLists.removeOne(it);
                delete it;
                it = nullptr;
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
