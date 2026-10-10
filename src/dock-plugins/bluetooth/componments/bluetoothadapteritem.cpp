// SPDX-FileCopyrightText: 2016 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothadapteritem.h"
#include "adapter.h"
#include "bluetoothconstants.h"
#include "refreshbutton.h"
#include "horizontalseparator.h"
#include "statebutton.h"
#include "plugins-logging-category.h"
#include "dconfig_helper.h"

#include <DFontSizeManager>
#include <DLabel>
#include <DSwitchButton>
#include <DListView>
#include <DSpinner>
#include <DPaletteHelper>
#include <DGuiApplicationHelper>

#include <QBoxLayout>
#include <QStandardItemModel>

BluetoothDeviceItem::BluetoothDeviceItem(QStyle *style, const Device *device, PluginListView *parent)
    : m_style(style)
    , m_device(device)
{
    Q_UNUSED(parent)
    m_standardItem = new PluginStandardItem();

    m_standardItem->updateIcon(m_device->deviceType().isEmpty() ? QIcon::fromTheme("bluetooth_other")
                                                                : QIcon::fromTheme(QString("bluetooth_%1").arg(m_device->deviceType())));

    updateDeviceState(m_device->state());

    initConnect();

    m_standardItem->updateBattery(m_device->battery());
    connect(m_device, &Device::batteryChanged, this, [this](int battery) {
        m_standardItem->updateBattery(battery);
    });
}

BluetoothDeviceItem::~BluetoothDeviceItem()
{
}

void BluetoothDeviceItem::initConnect()
{
    connect(m_device, &Device::stateChanged, this, &BluetoothDeviceItem::updateDeviceState);
    connect(m_standardItem, &PluginStandardItem::connectBtnClicked, this, &BluetoothDeviceItem::disconnectDevice);
}

void BluetoothDeviceItem::updateDeviceState(Device::State state)
{
    m_standardItem->updateName(m_device->alias());

    if (state == Device::StateAvailable) {
        m_standardItem->updateState(PluginItemState::Connecting);
    } else if (state == Device::StateConnected) {
        m_standardItem->updateState(PluginItemState::Connected);
        emit requestTopDeviceItem(m_standardItem);
    } else {
        m_standardItem->updateState(PluginItemState::NoState);
    }

    emit deviceStateChanged(m_device);
}

BluetoothAdapterItem::BluetoothAdapterItem(Adapter *adapter, QWidget *parent)
    : QWidget(parent)
    , m_adapter(adapter)
    , m_adapterLabel(new SettingLabel(adapter->name(), this))
    , m_adapterLayout(new QVBoxLayout)
    , m_adapterStateBtn(new DSwitchButton(this))
    , m_myDeviceWidget(new QWidget(this))
    , m_myDeviceLabel(new DLabel(tr("My Devices"), this))
    , m_myDeviceListView(new PluginListView(this))
    , m_myDeviceModel(new QStandardItemModel(m_myDeviceListView))
    , m_otherDeviceControlWidget(new DeviceControlWidget(this))
    , m_otherDeviceListView(new PluginListView(this))
    , m_otherDeviceModel(new QStandardItemModel(m_otherDeviceListView))
    , m_refreshBtn(new CommonIconButton(this))
    , m_bluetoothInter(new DBusBluetooth("org.deepin.dde.Bluetooth1", "/org/deepin/dde/Bluetooth1", QDBusConnection::sessionBus(), this))
    , m_showUnnamedDevices(false)
    , m_stateBtnEnabled(true)
    , m_adapterSwitchEnabled(true)
    , m_scanTimer(new QTimer(this))
{
    initUi();
    initConnect();
    initData();
}

BluetoothAdapterItem::~BluetoothAdapterItem()
{
    qDeleteAll(m_deviceItems);
}

void BluetoothAdapterItem::onConnectDevice(const QModelIndex &index)
{
    const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(index.model());
    if (!deviceModel)
        return;
    PluginStandardItem *deviceitem = dynamic_cast<PluginStandardItem *>(deviceModel->item(index.row()));

    foreach (const auto item, m_deviceItems) {
        // 只有非连接状态才发送connectDevice信号（connectDevice信号连接的槽为取反操作，而非仅仅连接）
        if (!(item->device()->state() == Device::StateUnavailable))
            continue;

        if (item->standardItem() == deviceitem) {
            emit connectDevice(item->device(), m_adapter);
        }
    }
}

void BluetoothAdapterItem::onTopDeviceItem(PluginStandardItem *item)
{
    if (!item || item->row() == -1 || item->row() == 0)
        return;

    int row = item->row();
    // 先获取，再移除，后插入
    PluginStandardItem *sItem = dynamic_cast<PluginStandardItem *>(m_myDeviceModel->takeItem(row, 0));
    if (sItem) {
        m_myDeviceModel->removeRow(row);
        m_myDeviceModel->insertRow(0, sItem);
    }
}

void BluetoothAdapterItem::onAdapterNameChanged(const QString name)
{
    m_adapterLabel->label()->setText(name);
}

QSize BluetoothAdapterItem::sizeHint() const
{
    const int myViewHeight = m_myDeviceModel->rowCount() * (m_myDeviceListView->getItemHeight() + m_myDeviceListView->getItemSpacing()) - m_myDeviceListView->getItemSpacing() + m_myDeviceLabel->height() + 10 + m_otherDeviceControlWidget->height();
    const int otherViewHeight = 5 + m_otherDeviceModel->rowCount() * (m_otherDeviceListView->getItemHeight() + m_otherDeviceListView->getItemSpacing()) - m_otherDeviceListView->getItemSpacing();
    const int viewHeight = (m_myDeviceWidget->isVisible() ? myViewHeight : 0) + (m_otherDeviceListView->isVisibleTo(this) ? otherViewHeight : 0);
    m_otherDeviceListView->setFixedHeight(otherViewHeight);
    const auto &margin = m_adapterLayout->contentsMargins();
    const int adatperHeight = qMax(m_adapterLabel->sizeHint().height(), m_adapterStateBtn->sizeHint().height()) + margin.top() + margin.bottom();
    return QSize(ItemWidth, adatperHeight + (m_adapter->powered() ? viewHeight : 5));
}

QStringList BluetoothAdapterItem::connectedDevicesName()
{
    QStringList devices;
    for (BluetoothDeviceItem *devItem : m_deviceItems) {
        if (devItem && devItem->device()->state() == Device::StateConnected) {
            devices << devItem->device()->alias();
        }
    }

    return devices;
}

void BluetoothAdapterItem::setStateBtnEnabled(bool enable)
{
    if (m_stateBtnEnabled != enable) {
        m_stateBtnEnabled = enable;
    }

    if (m_adapterStateBtn) {
        m_adapterStateBtn->setEnabled(m_stateBtnEnabled);
    }
}

void BluetoothAdapterItem::initData()
{
    m_showUnnamedDevices = m_bluetoothInter->displaySwitch();

    if (m_adapter && !m_adapter->powered())
        return;

    foreach (const auto device, m_adapter->devices()) {
        if (!m_deviceItems.contains(device))
            onDeviceAdded(device);
    }
    setUnnamedDevicesVisible(m_showUnnamedDevices);

    if (m_adapterSwitchEnabled) {
        emit deviceCountChanged();
    }
}

void BluetoothAdapterItem::onDeviceAdded(const Device *device)
{
    if (m_adapter && !m_adapter->powered()) {
        qCInfo(BLUETOOTH) << "Adapter powered is false.";
        return;
    }

    if (!m_showUnnamedDevices && device->name().isEmpty())
        return;

    BluetoothDeviceItem *deviceItem = new BluetoothDeviceItem(style(), device, m_otherDeviceListView);
    connect(deviceItem, &BluetoothDeviceItem::requestTopDeviceItem, this, &BluetoothAdapterItem::onTopDeviceItem);
    connect(deviceItem, &BluetoothDeviceItem::deviceStateChanged, this, [this, deviceItem] (const Device *device) {
        Q_EMIT deviceStateChanged(device);
        if (!(device->state() == Device::StateUnavailable))
            return;
        PluginStandardItem *item = deviceItem->standardItem();
        if (item && deviceItem->device()->paired()) {
            QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
            if(myDeviceIndex.isValid()) {
                PluginStandardItem *sItem = dynamic_cast<PluginStandardItem *>(m_myDeviceModel->takeItem(myDeviceIndex.row(), 0));
                if (sItem) {
                    QMap<const Device *, BluetoothDeviceItem *>::iterator i;
                    // 计算已连接蓝牙设备数
                    int connectCount = 0;
                    for (i = m_deviceItems.begin(); i != m_deviceItems.end(); ++i) {
                        BluetoothDeviceItem *deviceItem = i.value();

                        if (deviceItem && deviceItem->device() && deviceItem->device()->paired()
                                && (Device::StateConnected == deviceItem->device()->state() || deviceItem->device()->connecting()))
                            connectCount++;
                    }

                    m_myDeviceModel->removeRow(myDeviceIndex.row());
                    m_myDeviceModel->insertRow(connectCount, sItem);
                }
            }
        }
    });
    connect(deviceItem, &BluetoothDeviceItem::disconnectDevice, this, [this, deviceItem](){
        // 只有已连接状态才发送connectDevice信号（connectDevice信号连接的槽为取反操作，而非仅仅连接）
        if (deviceItem->device()->state() == Device::StateConnected) {
            emit connectDevice(deviceItem->device(), m_adapter);
        }
    });
    connect(device, &Device::pairedChanged, deviceItem, [this, deviceItem](const bool paired) {
        if (deviceItem && deviceItem->device()) {
            PluginStandardItem *item = deviceItem->standardItem();
            if (item) {
                if (paired) {
                    QModelIndex otherDeviceIndex = m_otherDeviceModel->indexFromItem(item);
                    if(otherDeviceIndex.isValid()) {
                        PluginStandardItem *sItem = dynamic_cast<PluginStandardItem *>(m_otherDeviceModel->takeItem(otherDeviceIndex.row(), 0));
                        if (sItem) {
                            m_otherDeviceModel->removeRow(otherDeviceIndex.row());
                            m_myDeviceModel->appendRow(sItem);
                        }
                    }
                } else {
                    QModelIndex myDeviceIndex = m_myDeviceModel->indexFromItem(item);
                    if (myDeviceIndex.isValid()) {
                        PluginStandardItem *sItem = dynamic_cast<PluginStandardItem *>(m_myDeviceModel->takeItem(myDeviceIndex.row(), 0));
                        if (sItem) {
                            m_myDeviceModel->removeRow(myDeviceIndex.row());
                            m_otherDeviceModel->appendRow(sItem);
                        }
                    }
                }
            }
        }
        m_myDeviceWidget->setVisible(m_myDeviceModel->rowCount() > 0 && m_adapterStateBtn->isChecked());

        if (m_adapterSwitchEnabled) {
            emit deviceCountChanged();
        }
    });

    m_deviceItems.insert(device, deviceItem);
    if (device->paired()) {
        if (device->state() != Device::StateConnected) {
            m_myDeviceModel->appendRow(deviceItem->standardItem());
        } else {
            m_myDeviceModel->insertRow(0, deviceItem->standardItem());
        }

    } else {
        m_otherDeviceModel->appendRow(deviceItem->standardItem());
    }

    if (m_adapterSwitchEnabled) {
        emit deviceCountChanged();
    }
    m_myDeviceWidget->setVisible(m_myDeviceModel->rowCount() > 0 && m_adapterStateBtn->isChecked());
}

void BluetoothAdapterItem::onDeviceRemoved(const Device *device)
{
    if (m_deviceItems.isEmpty())
        return;

    int row = -1;
    if (!m_deviceItems.value(device))
        return;

    row = m_deviceItems.value(device)->standardItem()->row();
    if ((row < 0) || (row > m_deviceItems.size() - 1)) {
        delete m_deviceItems.value(device);
        m_deviceItems.remove(device);
        return;
    }

    QStandardItemModel *sourceModel = m_deviceItems.value(device)->standardItem()->model();
    if (sourceModel) {
        for (int rowIndex = 0; rowIndex < sourceModel->rowCount(); ++rowIndex) {
            QModelIndex index = sourceModel->index(rowIndex, 0);
            auto item = dynamic_cast<PluginStandardItem *>(sourceModel->itemFromIndex(index));
            if (item == m_deviceItems.value(device)->standardItem()) {
                sourceModel->removeRow(rowIndex);
                break;
            }
        }
    }

    delete m_deviceItems.value(device);
    m_deviceItems.remove(device);

    if (m_adapterSwitchEnabled) {
        if (m_myDeviceModel->rowCount() < 1) {
            m_myDeviceWidget->setVisible(false);
            m_otherDeviceControlWidget->setExpandState(true);
        }
        emit deviceCountChanged();
    }
}

void BluetoothAdapterItem::onDeviceNameUpdated(const Device *device)
{
    if (m_deviceItems.isEmpty())
        return;

    // 修复蓝牙设备列表中，设备名称更新后未实时刷新的问题
    if (m_deviceItems.contains(device)) {
        BluetoothDeviceItem *item = m_deviceItems[device];
        if (item && !item->device()->alias().isEmpty()) {
            item->updateDeviceState(item->device()->state());
        }
    }
}

void BluetoothAdapterItem::initUi()
{
    m_refreshBtn->setFixedSize(16, 16);
    m_refreshBtn->setClickable(true);
    m_refreshBtn->setIcon(QIcon::fromTheme("refresh"));

    setAccessibleName(m_adapter->name());
    setContentsMargins(0, 0, 0, 0);
    m_adapterLayout->setContentsMargins(0, 0, 0, 5);
    m_adapterLayout->setSpacing(0);
    m_adapterLayout->addWidget(m_adapterLabel);
    m_adapterLabel->addButton(m_refreshBtn, 6);
    m_adapterLabel->addButton(m_adapterStateBtn, 0);
    m_adapterStateBtn->setObjectName("AdapterStateBtn");
    DFontSizeManager::instance()->bind(m_adapterLabel->label(), DFontSizeManager::T4);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_myDeviceListView->setAccessibleName("MyDeviceItemList");
    m_myDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_myDeviceListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_myDeviceListView->setModel(m_myDeviceModel);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout *myDeviceLayout = new QVBoxLayout(m_myDeviceWidget);
    myDeviceLayout->setSpacing(0);
    myDeviceLayout->setContentsMargins(0, 0, 0, 0);
    m_myDeviceLabel->setContentsMargins(10, 0, 0, 6);
    DFontSizeManager::instance()->bind(m_myDeviceLabel, DFontSizeManager::T10);
    updateMyDeviceLabelTheme();
    myDeviceLayout->addWidget(m_myDeviceLabel);
    myDeviceLayout->addWidget(m_myDeviceListView);

    m_otherDeviceListView->setItemSpacing(1);
    m_otherDeviceListView->setBeginningItemStyle(QStyleOptionViewItem::Beginning);
    m_otherDeviceListView->setMiddleItemStyle(QStyleOptionViewItem::Middle);
    m_otherDeviceListView->setEndItemStyle(QStyleOptionViewItem::End);
    m_otherDeviceListView->setAccessibleName("OtherDeviceItemList");
    m_otherDeviceListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_otherDeviceListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_otherDeviceListView->setModel(m_otherDeviceModel);
    m_otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    myDeviceLayout->addSpacing(10);
    myDeviceLayout->addWidget(m_otherDeviceControlWidget);

    mainLayout->addLayout(m_adapterLayout);
    mainLayout->addWidget(m_myDeviceWidget);
    mainLayout->addSpacing(4);
    mainLayout->addWidget(m_otherDeviceListView);
    mainLayout->addStretch();

    if (m_adapter->discover()) {
        m_refreshBtn->startRotate();
    }

    const bool adapterPowered = m_adapter->powered();
    m_refreshBtn->setVisible(adapterPowered);
    m_adapterStateBtn->setChecked(adapterPowered);
    m_otherDeviceListView->setVisible(adapterPowered && m_otherDeviceControlWidget->isExpand());
    m_myDeviceWidget->setVisible(m_myDeviceModel->rowCount() > 0 && adapterPowered);
}

void BluetoothAdapterItem::initConnect()
{
    m_scanTimer->setInterval(DConfigHelper::instance()->getConfig("org.deepin.dde.dock", "org.deepin.dde.dock.plugin.bluetooth", "", "scanInterval", 5).toInt() * 1000);
    DConfigHelper::instance()->bind("org.deepin.dde.dock", "org.deepin.dde.dock.plugin.bluetooth", "", this, "scanInterval", [this] (const QString&, const QVariant&, QObject*) {
        m_scanTimer->setInterval(DConfigHelper::instance()->getConfig("org.deepin.dde.dock", "org.deepin.dde.dock.plugin.bluetooth", "", "scanInterval", 5).toInt() * 1000);
    });
    connect(m_scanTimer, &QTimer::timeout, this, [this] {
        if (isVisible())
            emit requestRefreshAdapter(m_adapter);
        m_scanTimer->stop();
    });
    connect(m_adapter, &Adapter::deviceAdded, this, &BluetoothAdapterItem::onDeviceAdded);
    connect(m_adapter, &Adapter::deviceRemoved, this, &BluetoothAdapterItem::onDeviceRemoved);
    connect(m_adapter, &Adapter::deviceNameUpdated, this, &BluetoothAdapterItem::onDeviceNameUpdated);
    connect(m_adapter, &Adapter::nameChanged, this, &BluetoothAdapterItem::onAdapterNameChanged);
    connect(m_otherDeviceListView, &PluginListView::clicked, this, &BluetoothAdapterItem::onConnectDevice);
    connect(m_myDeviceListView, &PluginListView::clicked, this, &BluetoothAdapterItem::onConnectDevice);
    connect(m_adapter, &Adapter::discoveringChanged, this, [ = ](bool state) {
        if (state) {
            m_refreshBtn->startRotate();
        } else {
            m_refreshBtn->stopRotate();
            if (isVisible())
                m_scanTimer->start();
        }
    });

    connect(m_refreshBtn, &CommonIconButton::clicked, this, [ = ] {
        emit requestRefreshAdapter(m_adapter);
    });

    connect(m_adapter, &Adapter::poweredChanged, this, [ = ](bool state) {
        if (state) {
            // Initialize device list when bluetooth is powered on
            initData();
        } else {
            // Clear device list when bluetooth is powered off
            clearDeviceList();
        }
        m_refreshBtn->setVisible(state);
        m_myDeviceWidget->setVisible(m_myDeviceModel->rowCount() > 0 && state);
        m_otherDeviceListView->setVisible(state && (m_otherDeviceControlWidget->isExpand() || m_myDeviceModel->rowCount() < 1));
        m_adapterStateBtn->setChecked(state);
        m_adapterStateBtn->setEnabled(m_stateBtnEnabled);
        emit adapterPowerChanged();
    });
    connect(m_adapterStateBtn, &DSwitchButton::clicked, this, [ = ](bool state) {
        m_adapterSwitchEnabled = state;
        clearDeviceList();
        m_myDeviceWidget->setVisible(false);
        m_otherDeviceListView->setVisible(false);
        m_adapterStateBtn->setEnabled(false);
        m_refreshBtn->setVisible(state);
        emit requestSetAdapterPower(m_adapter, state);
    });
    connect(m_bluetoothInter, &DBusBluetooth::DisplaySwitchChanged, this, [ = ](bool value) {
        m_showUnnamedDevices = value;
        setUnnamedDevicesVisible(value);
    });
    connect(m_otherDeviceControlWidget, &DeviceControlWidget::expandStateChanged, this, [this] {
        m_otherDeviceListView->setVisible(m_adapter->powered() && m_otherDeviceControlWidget->isExpand());
        if (m_adapterSwitchEnabled) {
            emit deviceCountChanged();
        }
    });

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this]{
        updateMyDeviceLabelTheme();
        update();
    });
}

void BluetoothAdapterItem::setUnnamedDevicesVisible(bool isShow)
{
    QMap<const Device *, BluetoothDeviceItem *>::iterator i;

    if (isShow) {
        // 计算已连接蓝牙设备数
        int connectCount = 0;
        for (i = m_deviceItems.begin(); i != m_deviceItems.end(); ++i) {
            BluetoothDeviceItem *deviceItem = i.value();

            if (deviceItem && deviceItem->device() && deviceItem->device()->paired()
                    && (Device::StateConnected == deviceItem->device()->state() || deviceItem->device()->connecting()))
                connectCount++;
        }

        // 显示所有蓝牙设备
        for (i = m_deviceItems.begin(); i != m_deviceItems.end(); ++i) {
            BluetoothDeviceItem *deviceItem = i.value();

            if (deviceItem && deviceItem->device() && deviceItem->device()->name().isEmpty()) {
                PluginStandardItem *dListItem = deviceItem->standardItem();
                QStandardItemModel *model = i.value()->standardItem()->model();
                if (!model)
                    continue;
                QModelIndex index = model->indexFromItem(dListItem);
                if (!index.isValid()) {
                    model->insertRow(((connectCount > -1 && connectCount < m_deviceItems.count()) ? connectCount : 0), dListItem);
                }
            }
        }

        return;
    }

    for (i = m_deviceItems.begin(); i != m_deviceItems.end(); ++i) {
        BluetoothDeviceItem *deviceItem = i.value();

        // 将名称为空的蓝牙设备过滤,如果蓝牙正在连接或者已连接不过滤
        if (deviceItem && deviceItem->device() && deviceItem->device()->name().isEmpty()
                && (Device::StateConnected != deviceItem->device()->state() || !deviceItem->device()->connecting())) {
            PluginStandardItem *dListItem = deviceItem->standardItem();
            QStandardItemModel *model = i.value()->standardItem()->model();
            if (!model)
                continue;
            QModelIndex index = model->indexFromItem(dListItem);
            if (index.isValid()) {
                model->takeRow(index.row());
            }
        }
    }
}

void BluetoothAdapterItem::showEvent(QShowEvent *event)
{
    // 设置默认展开状态：只有在没有已配对设备时才展开其他设备列表
    if (m_adapterSwitchEnabled) {
        bool shouldExpand = m_myDeviceModel->rowCount() < 1;
        m_otherDeviceControlWidget->setExpandState(shouldExpand);
        m_otherDeviceListView->setVisible(shouldExpand);
    }
    
    emit deviceCountChanged();
    if (m_adapter->discover()) {
        return;
    } else if (m_adapter->powered()) {
        // 蓝牙适配器开启了，但是未开启搜索，刷新蓝牙设备列表
        emit requestRefreshAdapter(m_adapter);
    }

    QWidget::showEvent(event);
}

void BluetoothAdapterItem::updateMyDeviceLabelTheme()
{
    DPalette palette = DPaletteHelper::instance()->palette(m_myDeviceLabel);
    QColor textColor;
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        textColor = Qt::black;
    } else {
        textColor = Qt::white;
    }
    textColor.setAlphaF(0.60);
    palette.setColor(QPalette::WindowText, textColor);
    DPaletteHelper::instance()->setPalette(m_myDeviceLabel, palette);
}

void BluetoothAdapterItem::clearDeviceList()
{
    qDeleteAll(m_deviceItems);
    m_deviceItems.clear();
    m_myDeviceModel->clear();
    m_otherDeviceModel->clear();
}
