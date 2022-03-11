/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#include "networkpanel.h"
#include "utils.h"
#include "item/netitem.h"
#include "item/devicestatushandler.h"
#include "thememanager.h"

#include <NetworkManagerQt/WirelessDevice>

#include <DHiDPIHelper>
#include <DApplicationHelper>
#include <DDBusSender>
#include <DGuiApplicationHelper>
#include <DSwitchButton>
#include <dloadingindicator.h>

#include <QTimer>
#include <QScroller>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWidget>
#include <QDialog>
#include <QScopedPointer>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>

#define SWITCH_WIDTH 50
#define SWITCH_HEIGHT 24

NetworkPanel::NetworkPanel(QObject *parent)
    : QObject(parent)
    , m_wirelessScanTimer(new QTimer(this))
    , m_switchWire(true)
    , m_applet(new QScrollArea())
    , m_centerWidget(new QWidget(m_applet))
    , m_netListView(new DListView(m_centerWidget))
    , m_airplaneMode(new DBusAirplaneMode("com.deepin.daemon.AirplaneMode", "/com/deepin/daemon/AirplaneMode", QDBusConnection::systemBus(), this))
    , m_updateTimer(new QTimer(this))
{
    initUi();
    if (NetworkController::instance()->devices().isEmpty()) {
        // 收到activeConnectionChange信号时数据才全准备好
        connect(NetworkController::instance(), &NetworkController::activeConnectionChange, this, &NetworkPanel::initConnection);
    } else {
        initConnection();
    }
}

NetworkPanel::~NetworkPanel()
{
    NetItem *oldSelectItem = selectItem();
    if (oldSelectItem) {
        WirelessItem *item = static_cast<WirelessItem *>(oldSelectItem);
        item->onCancel();
    }
}

void NetworkPanel::initUi()
{
    m_netListView->setAccessibleName("list_network");
    m_netListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_netListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_netListView->setFrameShape(QFrame::NoFrame);
    m_netListView->setViewportMargins(0, 0, 0, 0);
    m_netListView->setItemSpacing(1);
    m_netListView->setMouseTracking(true);
    m_netListView->setItemMargins(QMargins(10, 0, 10, 0));
    m_netListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_netListView->setItemRadius(0);

    NetworkDelegate *delegate = new NetworkDelegate(m_netListView);
    delegate->setDBusAirplaneMode(m_airplaneMode);
    delegate->setMargins(QMargins(10, 0, 6, 0));
    m_netListView->setItemDelegate(delegate);
    connect(delegate, &NetworkDelegate::refreshClicked, this, &NetworkPanel::onRefreshClicked);
    connect(delegate, &NetworkDelegate::enabledClicked, this, &NetworkPanel::onEnabledClicked);
    connect(delegate, &NetworkDelegate::closeClicked, this, [] (const QModelIndex &index) {
        // 获取该行数据对应的设备
        NetItemType type = index.data(NetItemRole::TypeRole).value<NetItemType>();
        if (type == NetItemType::WiredViewItem) {
            WiredDevice *device = index.data(NetItemRole::DeviceDataRole).value<WiredDevice *>();
            WiredConnection *connection = static_cast<WiredConnection *>(index.data(NetItemRole::DataRole).value<void *>());
            if (device && connection) {
                if (connection->connected())
                    device->disconnectNetwork();
                else
                    device->connectNetwork(connection);
            }
        } else if (type == NetItemType::WirelessViewItem || type == NetItemType::WirelessHiddenViewItem) {
            WirelessDevice *device = index.data(NetItemRole::DeviceDataRole).value<WirelessDevice *>();
            AccessPoints *accessPoint = static_cast<AccessPoints *>(index.data(NetItemRole::DataRole).value<void *>());
            if (device && accessPoint) {
                if (device->activeAccessPoints() == accessPoint)
                    device->disconnectNetwork();
            }
        }
    });

    m_model = new QStandardItemModel(this);
    m_model->setSortRole(sortRole);
    m_netListView->setModel(m_model);

    QVBoxLayout *centerLayout = new QVBoxLayout(m_centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->addWidget(m_netListView);

    m_applet->setFixedWidth(PANELWIDTH);
    m_applet->setWidget(m_centerWidget);
    m_applet->setFrameShape(QFrame::NoFrame);
    m_applet->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_applet->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_centerWidget->setAutoFillBackground(false);
    m_applet->viewport()->setAutoFillBackground(false);
    m_applet->setVisible(false);
    m_applet->installEventFilter(this);
    m_applet->setFixedSize(PANELWIDTH, 0);

    setControlBackground();

    // 支持在触摸屏上滚动
    QScroller::grabGesture(m_netListView->window(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_netListView->window());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);
}

void NetworkPanel::initConnection()
{
    // 主题发生变化触发的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NetworkPanel::onUpdatePlugView);

    // 连接信号
    disconnect(NetworkController::instance(), nullptr, this, nullptr);
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceAdded, this, &NetworkPanel::onDeviceAdded);
    connect(networkController, &NetworkController::deviceRemoved, this, &NetworkPanel::onUpdatePlugView);
    connect(networkController, &NetworkController::connectivityChanged, this, &NetworkPanel::onUpdatePlugView);

    // 点击列表的信号
    connect(m_netListView, &DListView::pressed, this, &NetworkPanel::onClickListView);

    int wirelessScanInterval = Utils::SettingValue("com.deepin.dde.dock", QByteArray(), "wireless-scan-interval", 10).toInt() * 1000;
    m_wirelessScanTimer->setInterval(wirelessScanInterval);
    const QGSettings *gsetting = Utils::SettingsPtr("com.deepin.dde.dock", QByteArray(), this);
    if (gsetting)
        connect(gsetting, &QGSettings::changed, [ & ](const QString &key) {
            if (key == "wireless-scan-interval") {
                int interval = gsetting->get("wireless-scan-interval").toInt() * 1000;
                m_wirelessScanTimer->setInterval(interval);
            }
        });
    connect(m_wirelessScanTimer, &QTimer::timeout, [ & ] {
        QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
        for (NetworkDeviceBase *device : devices) {
            if (device->deviceType() == DeviceType::Wireless) {
                WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
                wirelessDevice->scanNetwork();
            }
        }
    });
    connect(m_updateTimer, &QTimer::timeout, this, &NetworkPanel::updateView);
    m_updateTimer->setInterval(200);
    m_updateTimer->setSingleShot(true);

    QTimer::singleShot(0, this, [ = ] {
        onDeviceAdded(networkController->devices());
        updateView();
    });
}

void NetworkPanel::onEnabledClicked(const QModelIndex &index, const bool enabled)
{
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    switch (itemType) {
    case NetItemType::DeviceControllViewItem: {
        QList<NetworkDeviceBase *> devices = index.data(DeviceDataRole).value<QList<NetworkDeviceBase *>>();
        for (NetworkDeviceBase *device : devices) {
            if (device->isEnabled() != enabled)
                device->setEnabled(enabled);
        }
        break;
    }
    case NetItemType::WiredControllViewItem:
    case NetItemType::WirelessControllViewItem: {
        NetworkDeviceBase *device = index.data(DeviceDataRole).value<NetworkDeviceBase *>();
        if (device && device->isEnabled() != enabled)
            device->setEnabled(enabled);
        break;
    }
    default:
        break;
    }
}

void NetworkPanel::onRefreshClicked(const QModelIndex &index)
{
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    if (itemType != NetItemType::WirelessControllViewItem)
        return;

    WirelessDevice *device = index.data(DeviceDataRole).value<WirelessDevice *>();
    if (device)
        device->scanNetwork();
}

void NetworkPanel::updateItems()
{
    auto findBaseController = [ this ] (DeviceType t)-> DeviceControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::DeviceControllViewItem)
                continue;

            DeviceControllItem *pBaseCtrlItem = static_cast<DeviceControllItem *>(item);
            if (pBaseCtrlItem->deviceType() == t)
                return pBaseCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWiredController = [ this ] (WiredDevice *device)-> WiredControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WiredControllViewItem)
                continue;

            WiredControllItem *wiredCtrlItem = static_cast<WiredControllItem *>(item);
            if (wiredCtrlItem->device() == device)
                return wiredCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWiredItem = [ this ] (WiredConnection *conn)-> WiredItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WiredViewItem)
                continue;

            WiredItem *wiredItem = static_cast<WiredItem *>(item);
            if (wiredItem->connection() == conn)
                return wiredItem;
        }

        return Q_NULLPTR;
    };

    auto findWirelessController = [ this ] (WirelessDevice *device)-> WirelessControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WirelessControllViewItem)
                continue;

            WirelessControllItem *wiredCtrlItem = static_cast<WirelessControllItem *>(item);
            if (wiredCtrlItem->device() == device)
                return wiredCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWirelessItem = [ this ] (const AccessPoints *ap, const WirelessDevice *device)-> WirelessItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WirelessViewItem)
                continue;

            WirelessItem *wirelessItem = static_cast<WirelessItem *>(item);
            const AccessPoints *apData = wirelessItem->accessPoint();
            const WirelessDevice *wirelessdevice = wirelessItem->wirelessDevice();
            if (apData == ap && wirelessdevice == device)
                return wirelessItem;
        }

        return Q_NULLPTR;
    };

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    QList<WiredDevice *> wiredDevices;
    QList<WirelessDevice *> wirelessDevices;
    for (NetworkDeviceBase *device : devices) {
        if (device->deviceType() == DeviceType::Wired) {
            WiredDevice *dev = static_cast<WiredDevice *>(device);
            wiredDevices << dev;
        } else if (device->deviceType() == DeviceType::Wireless) {
            WirelessDevice *dev = static_cast<WirelessDevice *>(device);
            wirelessDevices << dev;
        }
    }

    // 存在多个无线设备的情况下，需要显示总开关
    int sortIndex = 0;
    QList<NetItem *> items;
    if (wirelessDevices.size() > 1) {
        DeviceControllItem *ctrl = findBaseController(DeviceType::Wireless);
        if (!ctrl)
            ctrl = new DeviceControllItem(DeviceType::Wireless, m_netListView->viewport());
        else
            ctrl->updateView();

        ctrl->standardItem()->setData(sortIndex++, sortRole);
        ctrl->setDevices(devices);
        items << ctrl;
    }

    // 遍历当前所有的无线网卡
    auto accessPoints = [] (WirelessDevice *device) {
        if (device->isEnabled())
            return device->accessPointItems();

        return QList<AccessPoints *>();
    };

    for (WirelessDevice *device : wirelessDevices) {
        WirelessControllItem *ctrl = findWirelessController(device);
        if (!ctrl)
            ctrl = new WirelessControllItem(m_netListView->viewport(), static_cast<WirelessDevice *>(device));
        ctrl->updateView();

        ctrl->standardItem()->setData(sortIndex++, sortRole);
        items << ctrl;
        if (device->isEnabled() && !device->hotspotEnabled()) {
            QList<AccessPoints *> aps = accessPoints(device);
            // 按连接状态、强度、名称排序
            qSort(aps.begin(), aps.end(), [](AccessPoints *a, AccessPoints *b) {
                int aStatus = static_cast<int>(a->status()) & 3;
                int bStatus = static_cast<int>(b->status()) & 3;
                if (aStatus ^ bStatus)
                    return aStatus != 0;
                if (a->strength() == b->strength())
                    return a->ssid() <= b->ssid();
                return a->strength() > b->strength();
            });
            for (AccessPoints *ap : aps) {
                WirelessItem *apCtrl = findWirelessItem(ap, device);
                if (!apCtrl) {
                    apCtrl = new WirelessItem(m_netListView->viewport(), device, ap);
                    connect(apCtrl, &WirelessItem::sizeChanged, this, &NetworkPanel::refreshItems);
                    connect(m_airplaneMode, &DBusAirplaneMode::EnabledChanged, apCtrl, &WirelessItem::onAirplaneModeChanged);
                }
                apCtrl->updateView();
                apCtrl->onAirplaneModeChanged(m_airplaneMode->enabled());

                apCtrl->standardItem()->setData(sortIndex++, sortRole);
                items << apCtrl;
            }
            if (!m_airplaneMode->enabled()) {
                // 连接隐藏网络
                WirelessItem *apCtrl = findWirelessItem(nullptr, device);
                if (!apCtrl) {
                    apCtrl = new WirelessItem(m_netListView->viewport(), device, nullptr);
                    connect(apCtrl, &WirelessItem::sizeChanged, this, &NetworkPanel::refreshItems);
                }
                apCtrl->updateView();

                apCtrl->standardItem()->setData(sortIndex++, sortRole);
                items << apCtrl;
            }
        }
    }

    // 存在多个有线设备的情况下，需要显示总开关
    if (wiredDevices.size() > 1) {
        DeviceControllItem *ctrl = findBaseController(DeviceType::Wired);
        if (!ctrl)
            ctrl = new DeviceControllItem(DeviceType::Wired, m_netListView->viewport());
        ctrl->updateView();

        ctrl->standardItem()->setData(sortIndex++, sortRole);
        ctrl->setDevices(devices);
        items << ctrl;
    }

    auto wiredConnections = [ & ](WiredDevice *device) {
        if (device->isEnabled())
            return device->items();

        return QList<WiredConnection *>();
    };

    // 遍历当前所有的有线网卡
    for (WiredDevice *device : wiredDevices) {
        WiredControllItem *ctrl = findWiredController(device);
        if (!ctrl)
            ctrl = new WiredControllItem(m_netListView->viewport(), device);
        ctrl->updateView();

        ctrl->standardItem()->setData(sortIndex++, sortRole);
        items << ctrl;

        QList<WiredConnection *> connItems = wiredConnections(device);
        for (WiredConnection *conn : connItems) {
            WiredItem *connectionCtrl = findWiredItem(conn);
            if (!connectionCtrl)
                connectionCtrl = new WiredItem(m_netListView->viewport(), device, conn);
            connectionCtrl->updateView();

            connectionCtrl->standardItem()->setData(sortIndex++, sortRole);
            items << connectionCtrl;
        }
    }

    // 把原来列表中不存在的项放到移除列表中
    for (NetItem *item : m_items) {
        if (!items.contains(item))
            delete item;
    }

    m_items = items;
}

void NetworkPanel::updateView()
{
    updateItems();
    refreshItems();
    passwordError(QString(), QString());
    if (m_items.isEmpty()) {
        // 1s后依旧无网络设备则退出
        QTimer::singleShot(1000, this, [ this ] {
            if (m_items.isEmpty())
                qApp->quit();
        });
    }
    // 只有当列表中的数量大于0的情况下，才抛出更新完成的信号,通知网络列表显示
    if (m_model->rowCount() > 0)
        Q_EMIT updateFinished();
}

void NetworkPanel::refreshItems()
{
    QList<QStandardItem *> items;
    QList<int> rmRows;
    for (NetItem *item : m_items)
        items << item->standardItem();

    for (int i = 0; i < m_model->rowCount(); i++) {
        DStandardItem *item = static_cast<DStandardItem *>(m_model->item(i));
        if (!items.contains(item))
            rmRows << i;
    }
    // 将row按照从大到小的顺序排序，否则会出现删除错误的问题
    qSort(rmRows.begin(), rmRows.end(), [=](int &row1, int &row2) { return row1 > row2; });
    for (int row : rmRows)
        m_model->removeRow(row);

    // 从缓存中查找出当前列表不存在的Item,并插入到列表中
    QList<QStandardItem *> currentItems;
    for (int i = 0; i < m_model->rowCount(); i++)
        currentItems << m_model->item(i);

    QList<QStandardItem *> newItems;
    for (QStandardItem *item : items) {
        if (!currentItems.contains(item))
            newItems << item;
    }

    for (QStandardItem *item : newItems)
        m_model->appendRow(item);

    // 对列表进行重新排序
    m_model->sort(0);

    // 设置高度
    int height = 0;
    int totalHeight = 0;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QStandardItem *item = m_model->item(i);
        QSize size = item->sizeHint();
        if (i < 16)
            height += size.height();

        totalHeight += size.height();
    }

    m_netListView->setFixedSize(PANELWIDTH, totalHeight);
    m_centerWidget->setFixedSize(PANELWIDTH, totalHeight);
    m_applet->setFixedSize(PANELWIDTH, height);

    m_netListView->update();
}

bool NetworkPanel::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_applet) {
        switch (event->type()) {
        case QEvent::Show: {
            if (!m_wirelessScanTimer->isActive())
                m_wirelessScanTimer->start();
            break;
        }
        case QEvent::Hide: {
            if (m_wirelessScanTimer->isActive())
                m_wirelessScanTimer->stop();
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(obj, event);
}

int NetworkPanel::deviceCount(const DeviceType &devType)
{
    // 获取指定的设备类型的设备数量
    int count = 0;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *dev : devices)
        if (dev->deviceType() == static_cast<DeviceType>(devType))
            count++;

    return count;
}

void NetworkPanel::onDeviceAdded(QList<NetworkDeviceBase *> devices)
{
    // 处理新增设备的信号
    for (NetworkDeviceBase *device : devices) {
        // 当网卡连接状态发生变化的时候重新绘制任务栏的图标
        connect(device, &NetworkDeviceBase::deviceStatusChanged, this, &NetworkPanel::onUpdatePlugView);
        connect(device, &NetworkDeviceBase::activeConnectionChanged, this, &NetworkPanel::onUpdatePlugView);
        switch (device->deviceType()) {
        case DeviceType::Wired: {
            WiredDevice *wiredDevice = static_cast<WiredDevice *>(device);

            connect(wiredDevice, &WiredDevice::connectionAdded, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &WiredDevice::connectionRemoved, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &WiredDevice::connectionPropertyChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::deviceStatusChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::enableChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::connectionChanged, this, &NetworkPanel::onUpdatePlugView);
        } break;
        case DeviceType::Wireless: {
            WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);

            connect(wirelessDevice, &WirelessDevice::networkAdded, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::networkRemoved, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::accessPointInfoChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::enableChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::connectionChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::hotspotEnableChanged, this, &NetworkPanel::onUpdatePlugView);

            wirelessDevice->scanNetwork();
        } break;
        default:
            break;
        }
    }

    onUpdatePlugView();
}

bool NetworkPanel::deviceEnabled(const DeviceType &deviceType) const
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices)
        if (device->deviceType() == deviceType && device->isEnabled())
            return true;

    return false;
}

void NetworkPanel::setDeviceEnabled(const DeviceType &deviceType, bool enabeld)
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices)
        if (device->deviceType() == deviceType)
            device->setEnabled(enabeld);
}

QWidget *NetworkPanel::itemApplet()
{
    return m_applet;
}

void NetworkPanel::setControlBackground()
{
    QPalette backgroud = m_applet->palette();
    backgroud.setColor(QPalette::Background, ThemeManager::instance()->backgroundColor());
    m_applet->setAutoFillBackground(true);
    m_applet->setPalette(backgroud);
}

void NetworkPanel::onUpdatePlugView()
{
    if (!m_updateTimer->isActive()) {
        m_updateTimer->start();
    }
}

void NetworkPanel::onClickListView(const QModelIndex &index)
{
    // 如果当前点击的是连接隐藏网络或者无线网络，且开启了飞行模式，则不让点击
    NetItemType type = index.data(NetItemRole::TypeRole).value<NetItemType>();
    if ((type == WirelessHiddenViewItem || type == WirelessViewItem) && m_airplaneMode->enabled())
        return;

    NetItem *oldSelectItem = selectItem();
    NetItem *newSelectItem = m_items.at(index.row());
    if (newSelectItem != oldSelectItem && oldSelectItem) {
        WirelessItem *item = static_cast<WirelessItem *>(oldSelectItem);
        item->expandWidget(WirelessItem::Hide); // 选择切换时隐藏输入框
    }
    switch (type) {
    case WirelessHiddenViewItem:
    case WirelessViewItem: {
        WirelessItem *item = static_cast<WirelessItem *>(newSelectItem);
        item->connectNetwork();
        break;
    }
    case WiredViewItem: {
        WiredItem *item = static_cast<WiredItem *>(newSelectItem);
        item->connectNetwork();
        break;
    }
    default:
        break;
    }
}

int NetworkPanel::getStrongestAp()
{
    int retStrength = -1;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices) {
        if (device->deviceType() != DeviceType::Wireless)
            continue;

        WirelessDevice *dev = static_cast<WirelessDevice *>(device);
        AccessPoints *ap = dev->activeAccessPoints();
        if (ap && retStrength < ap->strength())
            retStrength = ap->strength();
    }

    return retStrength;
}

void NetworkPanel::passwordError(const QString &dev, const QString &ssid)
{
    if (!ssid.isEmpty()) {
        m_reconnectSsid = ssid;
        m_reconnectDev = dev;
    }
    if (!m_reconnectSsid.isEmpty()) {
        QTimer::singleShot(0, this, &NetworkPanel::expandPasswordInput);
    }
}

void NetworkPanel::expandPasswordInput()
{
    if (m_reconnectSsid.isEmpty()) {
        return;
    }
    NetItem *oldSelectItem = selectItem();
    int y = 0;
    for (NetItem *item : m_items) {
        if (NetItemType::WirelessViewItem == item->itemType()) {
            WirelessItem *wirelessItem = static_cast<WirelessItem *>(item);
            if (wirelessItem->accessPoint()
                && (m_reconnectDev.isEmpty() || wirelessItem->wirelessDevice()->path() == m_reconnectDev)
                && (wirelessItem->standardItem()->text() == m_reconnectSsid)) {
                if (item != oldSelectItem) {
                    if (oldSelectItem) {
                        WirelessItem *selectItem = static_cast<WirelessItem *>(oldSelectItem);
                        selectItem->expandWidget(WirelessItem::Hide); // 选择切换时隐藏输入框
                    }
                    wirelessItem->expandPasswordInput();
                    int h = item->standardItem()->sizeHint().height() / 2;
                    m_applet->ensureVisible(0, y + h, 0, h);
                }
                m_reconnectSsid.clear();
                m_reconnectDev.clear();
                break;
            }
        }
        y += item->standardItem()->sizeHint().height();
    }
}

NetItem *NetworkPanel::selectItem()
{
    for (NetItem *item : m_items) {
        if (NetItemType::WirelessViewItem == item->itemType() || NetItemType::WirelessHiddenViewItem == item->itemType()) {
            WirelessItem *wirelessItem = static_cast<WirelessItem *>(item);
            if (wirelessItem->expandVisible()) {
                return wirelessItem;
            }
        }
    }
    return nullptr;
}

// 用于绘制分割线
#define RIGHTMARGIN 13
#define DIAMETER 16

NetworkDelegate::NetworkDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
    , m_parentWidget(parent)
    , m_refreshIconTimer(new QTimer(this))
    , m_currentDegree(0)
    , m_refreshTimer(new QTimer(this))
    , m_airplaneMode(nullptr)
{
    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        this->m_currentDegree += 14;
        m_parentWidget->update();
    });

    m_refreshIconTimer->setInterval(3);
    connect(m_refreshIconTimer, &QTimer::timeout, this, [ this ] {
        QList<QModelIndex> refreshIndexs = m_refreshAngle.keys();
        for (int i = refreshIndexs.size() - 1; i >= 0; i--) {
            const QModelIndex &index = refreshIndexs[i];
            int angle = m_refreshAngle[index];
            if (angle >= 360) {
                m_refreshAngle.remove(index);
            } else {
                angle += (angle <= 180 ? 5 : 8);
                m_refreshAngle[index] = angle;
            }
        }
        if (m_refreshAngle.size() == 0) {
            if (m_refreshIconTimer->isActive())
                m_refreshIconTimer->stop();
        } else {
            m_parentWidget->update();
        }
    });

    m_refreshTimer->setInterval(30);
}

NetworkDelegate::~NetworkDelegate()
{
    if (m_refreshTimer->isActive())
        m_refreshTimer->stop();
}

void NetworkDelegate::setDBusAirplaneMode(DBusAirplaneMode *airplane)
{
    m_airplaneMode = airplane;
}

void NetworkDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    option->state &= ~QStyle::State_MouseOver;
    DStyledItemDelegate::initStyleOption(option, index);
}

void NetworkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool drawLine = needDrawLine(index);
    if (drawLine) {
        // 绘制间隔线
        QRect rct = option.rect;
        rct.setY(rct.top() + rct.height() - 2);
        rct.setHeight(2);
        painter->fillRect(rct, ThemeManager::instance()->lineColor());
    }
    // 鼠标移动的时候不
    bool isHoverItem = cantHover(index);
    QRect rect = option.rect;
    if (drawLine)
        rect.setHeight(rect.height() - 2);

    if (!isHoverItem && (option.state & QStyle::State_MouseOver)) {
        painter->fillRect(rect, ThemeManager::instance()->itemBackgroundColor());
    }

    // 绘制无线网络左侧的刷新按钮
    if (index.data(TypeRole).value<NetItemType>() == NetItemType::WirelessControllViewItem)
        drawRefreshButton(painter, option, index);

    // 绘制开关按钮，因为如果直接在DStandardItem上使用DSwitchButton就会导致更新列表的时候引起错位，就使用绘制开关的方式来实现吧
    if (hasSwitchButton(index))
        drawSwitchButton(painter, option, index);

    // 绘制右侧的连接图标
    NetConnectionType connectionStatus = static_cast<NetConnectionType>(index.data(NetItemRole::ConnectionStatusRole).toInt());
    if (connectionStatus == NetConnectionType::Connecting) {
        if (!m_ConnectioningIndexs.contains(index))
            m_ConnectioningIndexs << index;
    } else {
        if (m_ConnectioningIndexs.contains(index))
            m_ConnectioningIndexs.removeOne(index);
    }

    if (m_ConnectioningIndexs.size() > 0) {
        if (!m_refreshTimer->isActive())
            m_refreshTimer->start();
    } else {
        if (m_refreshTimer->isActive()) {
            m_refreshTimer->stop();
            m_currentDegree = 0;
        }
    }

    switch (connectionStatus) {
    case NetConnectionType::Connected: {
        painter->setRenderHint(QPainter::Antialiasing, true);

        QRect rct = checkRect(option.rect);
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(m_parentWidget->palette().color(QPalette::Highlight));

        QPen pen(Qt::white, DIAMETER / 100.0 * 6.20, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        if (index.data(NetItemRole::MouseInBoundingRole).toBool())
            drawFork(painter, rct, pen, DIAMETER);
        else
            drawCheck(painter, rct, pen, DIAMETER);

        break;
    }
    case NetConnectionType::Connecting: {
        QRect rct = checkRect(option.rect);
        drawLoading(painter, rct, DIAMETER);
        break;
    }
    default:
        break;
    }

    DStyledItemDelegate::paint(painter, option, index);
}

bool NetworkDelegate::needDrawLine(const QModelIndex &index) const
{
    // 如果是最后一行，则无需绘制线条
    QModelIndex siblingIndex = index.siblingAtRow(index.row() + 1);
    if (!siblingIndex.isValid())
        return false;

    // 如果是总控开关，无线开关和有线开关，下面都要分割线
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    if (itemType == NetItemType::DeviceControllViewItem
        || itemType == NetItemType::WirelessControllViewItem
        || itemType == NetItemType::WiredControllViewItem)
        return true;

    NetItemType nextItemType = siblingIndex.data(TypeRole).value<NetItemType>();
    return itemType != nextItemType;
}

bool NetworkDelegate::cantHover(const QModelIndex &index) const
{
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    // 如果是无线网络或者连接隐藏网络项，且当前开启了飞行模式，则当前行不让点击
    if (itemType == NetItemType::WirelessViewItem || itemType == NetItemType::WirelessHiddenViewItem)
        return (m_airplaneMode && m_airplaneMode->enabled());

    return (itemType == NetItemType::DeviceControllViewItem
            || itemType == NetItemType::WirelessControllViewItem
            || itemType == NetItemType::WiredControllViewItem);
}

bool NetworkDelegate::hasSwitchButton(const QModelIndex &index) const
{
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    // 如果是总控、有线网卡、无线网卡开关，则需要显示开关
    return (itemType == NetItemType::DeviceControllViewItem
            || itemType == NetItemType::WirelessControllViewItem
            || itemType == NetItemType::WiredControllViewItem);
}

void NetworkDelegate::drawCheck(QPainter *painter, QRect &rect, QPen &pen, int radius) const
{
    painter->drawPie(rect, 0, 360 * 16);
    painter->setPen(pen);

    QPointF points[3] = {
        QPointF(rect.left() + radius / 100.0 * 32, rect.top() + radius / 100.0 * 57),
        QPointF(rect.left() + radius / 100.0 * 45, rect.top() + radius / 100.0 * 70),
        QPointF(rect.left() + radius / 100.0 * 75, rect.top() + radius / 100.0 * 35)
    };

    painter->drawPolyline(points, 3);
}

void NetworkDelegate::drawFork(QPainter *painter, QRect &rect, QPen &pen, int radius) const
{
    painter->drawPie(rect, 0, 360 * 16);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    QPointF pointsl[2] = {
        QPointF(rect.left() + radius / 100.0 * 35, rect.top() + radius / 100.0 * 35),
        QPointF(rect.left() + radius / 100.0 * 65, rect.top() + radius / 100.0 * 65)
    };

    painter->drawPolyline(pointsl, 2);

    QPointF pointsr[2] = {
        QPointF(rect.left() + radius / 100.0 * 65, rect.top() + radius / 100.0 * 35),
        QPointF(rect.left() + radius / 100.0 * 35, rect.top() + radius / 100.0 * 65)
    };

    painter->drawPolyline(pointsr, 2);
}

void NetworkDelegate::drawLoading(QPainter *painter, QRect &rect, int diameter) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QList<QList<QColor>> indicatorColors;
    for (int i = 0; i < 3; i++)
        indicatorColors << createDefaultIndicatorColorList(m_parentWidget->palette().highlight().color());

    double radius = diameter * 0.66;
    auto center = QRectF(rect).center();
    auto indicatorRadius = radius / 2 / 2 * 1.1;
    auto indicatorDegreeDelta = 360 / indicatorColors.count();

#define INDICATOR_SHADOW_OFFSET 10

    for (int i = 0; i < indicatorColors.count(); ++i) {
        QList<QColor> colors = indicatorColors.value(i);
        for (int j = 0; j < colors.count(); ++j) {
            double degreeCurrent = m_currentDegree - j * INDICATOR_SHADOW_OFFSET + indicatorDegreeDelta * i;
            auto x = (radius - indicatorRadius) * qCos(qDegreesToRadians(degreeCurrent));
            auto y = (radius - indicatorRadius) * qSin(qDegreesToRadians(degreeCurrent));

            x = center.x() + x;
            y = center.y() + y;
            auto tl = QPointF(x - 1 * indicatorRadius, y - 1 * indicatorRadius);
            QRectF rf(tl.x(), tl.y(), indicatorRadius * 2, indicatorRadius * 2);

            QPainterPath path;
            path.addEllipse(rf);

            painter->fillPath(path, colors.value(j));
        }
    }
}

bool NetworkDelegate::switchIsEnabled(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    switch (itemType) {
    case NetItemType::DeviceControllViewItem: {
        QList<NetworkDeviceBase *> devices = index.data(NetItemRole::DeviceDataRole).value<QList<NetworkDeviceBase *>>();
        for (NetworkDeviceBase *device : devices) {
            if (device->isEnabled())
                return true;
        }
        return false;
    }
    case NetItemType::WiredControllViewItem:
    case NetItemType::WirelessControllViewItem: {
        NetworkDeviceBase *device = index.data(NetItemRole::DeviceDataRole).value<NetworkDeviceBase *>();
        if (device)
            return device->isEnabled();
        break;
    }
    default:
        break;
    }
    return false;
}

void NetworkDelegate::drawRefreshButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 如果当前开关为关闭状态，则不显示刷新按钮
    if (!switchIsEnabled(index))
        return;

    QRect rctIcon(option.rect.width() - SWITCH_WIDTH - 36, option.rect.top() + (option.rect.height() - 20) / 2, 20, 20);
    QPixmap pixmap = DHiDPIHelper::loadNxPixmap(ThemeManager::instance()->getIcon("wireless/refresh"));
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (m_refreshAngle.contains(index)) {
        QPoint ptCenter(rctIcon.left() + rctIcon.width() / 2, rctIcon.top() + rctIcon.height() / 2);
        painter->translate(ptCenter);
        painter->rotate(m_refreshAngle[index]);
        painter->drawPixmap(QRect(-10, -10, 20, 20), pixmap);
    } else {
        painter->drawPixmap(rctIcon, pixmap);
    }

    painter->restore();
}

void NetworkDelegate::drawSwitchButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QRect rctSwitch(option.rect.width() - SWITCH_WIDTH - 10,
                    option.rect.top() + (option.rect.height() - SWITCH_HEIGHT) / 2,
                    SWITCH_WIDTH, SWITCH_HEIGHT);
    painter->setPen(Qt::NoPen);
    DPalette palette = DGuiApplicationHelper::instance()->applicationPalette();
    painter->setBrush(palette.color(DPalette::ColorRole::Background));
    painter->drawRoundedRect(rctSwitch, 8, 8);
    bool isSwitchEnabled = switchIsEnabled(index);
    NetItemType itemType = index.data(TypeRole).value<NetItemType>();
    // 如果是总控、有线网卡、无线网卡开关，则需要显示开关
    QPalette::ColorRole colorRole = isSwitchEnabled ? QPalette::ColorRole::Highlight : DPalette::ColorRole::WindowText;
    if (m_airplaneMode->enabled() && itemType == NetItemType::WirelessControllViewItem)
        painter->setBrush(palette.color(QPalette::ColorGroup::Disabled, colorRole));
    else
        painter->setBrush(palette.color(colorRole));

    isSwitchEnabled ? rctSwitch.setLeft(rctSwitch.left() + 20) : rctSwitch.setWidth(30);
    painter->drawRoundedRect(rctSwitch, 8, 8);

    painter->restore();
}

bool NetworkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseMove: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QRect rct = checkRect(option.rect);
        model->setData(index, rct.contains(mouseEvent->pos()), NetItemRole::MouseInBoundingRole);
        m_parentWidget->update();
        break;
    }
    case QEvent::Leave: {
        model->setData(index, false, NetItemRole::MouseInBoundingRole);
        m_parentWidget->update();
        break;
    }
    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (index.data(TypeRole).value<NetItemType>() == NetItemType::WirelessControllViewItem) {
            if (!m_airplaneMode->enabled()) {
                if (!m_refreshAngle.contains(index)) {
                    QRect rctSwitch(option.rect.width() - SWITCH_WIDTH - 36, option.rect.top() + (option.rect.height() - 20) / 2, 20, 20);
                    if (rctSwitch.contains(mouseEvent->pos())) {
                        Q_EMIT refreshClicked(index);
                        // 后面刷新图标是根据m_refreshAngle列表中是否存在来决定是否刷新的，因此，需要提前将m_refreshAngle填充数据
                        m_refreshAngle[index] = 0;
                        if (!m_refreshIconTimer->isActive())
                            m_refreshIconTimer->start();
                        return true;
                    }
                }
            }
        }
        if (hasSwitchButton(index)) {
            NetItemType itemType = index.data(TypeRole).value<NetItemType>();
            // 以下三种情况可以点击按钮
            // 1: 飞行模式关闭 2: 当前是有线网卡 3: 当前是有线网卡总控
            if (!m_airplaneMode->enabled() || itemType == NetItemType::WiredControllViewItem
                || (itemType == NetItemType::DeviceControllViewItem && index.data(NetItemRole::DeviceTypeRole).value<DeviceType>() == DeviceType::Wired)) {
                QRect rctSwitch(option.rect.width() - SWITCH_WIDTH - 10,
                                option.rect.top() + (option.rect.height() - SWITCH_HEIGHT) / 2,
                                SWITCH_WIDTH, SWITCH_HEIGHT);
                if (rctSwitch.contains(mouseEvent->pos())) {
                    Q_EMIT enabledClicked(index, !switchIsEnabled(index));
                    return true;
                }
            }
        } else {
            NetConnectionType connectionStatus = static_cast<NetConnectionType>(index.data(NetItemRole::ConnectionStatusRole).toInt());
            QRect rct = checkRect(option.rect);
            if (connectionStatus == NetConnectionType::Connected) {
                if (rct.contains(mouseEvent->pos())) {
                    Q_EMIT closeClicked(index);
                    return true;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    return DStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect NetworkDelegate::checkRect(const QRect &rct) const
{
    int left = rct.right() - RIGHTMARGIN - DIAMETER + 4;
    int top = rct.top() + (DIAMETER / 2); // 连接图标绘制在顶端
    QRect rect;
    rect.setLeft(left);
    rect.setTop(top);
    rect.setWidth(DIAMETER);
    rect.setHeight(DIAMETER);
    return rect;
}

QList<QColor> NetworkDelegate::createDefaultIndicatorColorList(QColor color) const
{
    QList<QColor> colors;
    QList<int> opacitys;
    opacitys << 100 << 30 << 15 << 10 << 5 << 4 << 3 << 2 << 1;
    for (int i = 0; i < opacitys.count(); ++i) {
        color.setAlpha(255 * opacitys.value(i) / 100);
        colors << color;
    }
    return colors;
}
