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
#include "constants.h"
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
#include <hotspotcontroller.h>

NetworkPanel::NetworkPanel(QObject *parent)
    : QObject(parent)
    , m_switchWireTimer(new QTimer(this))
    , m_wirelessScanTimer(new QTimer(this))
    , m_switchWire(true)
    , m_applet(new QScrollArea())
    , m_centerWidget(new QWidget(m_applet))
    , m_netListView(new DListView(m_centerWidget))
    , m_timeOut(true)
    , m_selectItem(nullptr)
{
    initUi();
    initConnection();
}

NetworkPanel::~NetworkPanel()
{
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

    NetworkDelegate *delegate = new NetworkDelegate(m_netListView);
    delegate->setMargins(QMargins(10, 0, 10, 0));
    m_netListView->setItemDelegate(delegate);
    connect(delegate, &NetworkDelegate::closeClicked, this, [ ] (const QModelIndex &index) {
        // 获取该行数据对应的设备
        NetItemType type = static_cast<NetItemType>(index.data(NetItemRole::TypeRole).toInt());
        if (type == NetItemType::WiredViewItem) {
            WiredDevice *device = static_cast<WiredDevice *>(index.data(NetItemRole::DeviceDataRole).value<void *>());
            WiredConnection *connection = static_cast<WiredConnection *>(index.data(NetItemRole::DataRole).value<void *>());
            if (device && connection) {
                if (connection->connected())
                    device->disconnectNetwork();
                else
                    device->connectNetwork(connection);
            }
        } else if (type == NetItemType::WirelessViewItem) {
            WirelessDevice *device = static_cast<WirelessDevice *>(index.data(NetItemRole::DeviceDataRole).value<void *>());
            AccessPoints *accessPoint = static_cast<AccessPoints *>(index.data(NetItemRole::DataRole).value<void *>());
            if (device && accessPoint) {
                if (device->activeAccessPoints() == accessPoint)
                    device->disconnectNetwork();
            }
        }
    });

    m_model = new QStandardItemModel(this);
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

    setControlBackground();
}

void NetworkPanel::initConnection()
{
    // 主题发生变化触发的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NetworkPanel::onUpdatePlugView);

    // 连接信号
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceAdded, this, &NetworkPanel::onDeviceAdded);
    connect(networkController, &NetworkController::deviceRemoved, this, &NetworkPanel::onUpdatePlugView);
    connect(networkController, &NetworkController::connectivityChanged, this, &NetworkPanel::onUpdatePlugView);

    // 点击列表的信号
    connect(m_netListView, &DListView::pressed, this, &NetworkPanel::onClickListView);

    // 连接超时的信号
    connect(m_switchWireTimer, &QTimer::timeout, [ = ] () {
        m_switchWire = !m_switchWire;
        m_timeOut = true;
    });

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
    QTimer::singleShot(100, this, [ = ] {
        onDeviceAdded(networkController->devices());
    });
}

void NetworkPanel::getPluginState()
{
    // 所有设备状态叠加
    QList<int> status;
    m_pluginState = DeviceStatusHandler::pluginState();
    switch (m_pluginState) {
    case PluginState::Unknow:
    case PluginState::Disabled:
    case PluginState::Connected:
    case PluginState::Disconnected:
    case PluginState::ConnectNoInternet:
    case PluginState::WirelessDisabled:
    case PluginState::WiredDisabled:
    case PluginState::WirelessConnected:
    case PluginState::WiredConnected:
    case PluginState::WirelessDisconnected:
    case PluginState::WiredDisconnected:
    case PluginState::WirelessConnecting:
    case PluginState::WiredConnecting:
    case PluginState::WirelessConnectNoInternet:
    case PluginState::WiredConnectNoInternet:
    case PluginState::WiredFailed:
    case PluginState::Nocable:
        m_switchWireTimer->stop();
        m_timeOut = true;
        break;
    case PluginState::Connecting:
        // 启动2s切换计时,只有当计时器记满则重新计数
        if (m_timeOut) {
            m_switchWireTimer->start(2000);
            m_timeOut = false;
        }
        break;
    default:
        break;
    }
}

void NetworkPanel::updateItems()
{
    auto findBaseController = [ = ] (DeviceType t)-> DeviceControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::DeviceControllViewItem)
                continue;

            DeviceControllItem *pBaseCtrlItem = static_cast<DeviceControllItem *>(item);
            if (pBaseCtrlItem->deviceType() == t)
                return pBaseCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWiredController = [ = ] (WiredDevice *device)-> WiredControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WiredControllViewItem)
                continue;

            WiredControllItem *wiredCtrlItem = static_cast<WiredControllItem *>(item);
            if (wiredCtrlItem->device() == device)
                return wiredCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWiredItem = [ = ] (WiredConnection *conn)-> WiredItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WiredViewItem)
                continue;

            WiredItem *wiredItem = static_cast<WiredItem *>(item);
            if (wiredItem->connection() == conn)
                return wiredItem;
        }

        return Q_NULLPTR;
    };

    auto findWirelessController = [ = ] (WirelessDevice *device)-> WirelessControllItem * {
        for (NetItem *item : m_items) {
            if (item->itemType() != NetItemType::WirelessControllViewItem)
                continue;

            WirelessControllItem *wiredCtrlItem = static_cast<WirelessControllItem *>(item);
            if (wiredCtrlItem->device() == device)
                return wiredCtrlItem;
        }

        return Q_NULLPTR;
    };

    auto findWirelessItem = [ = ] (const AccessPoints *ap, const WirelessDevice *device)-> WirelessItem * {
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
    QList<NetItem *> items;
    if (wirelessDevices.size() > 1) {
        DeviceControllItem *ctrl = findBaseController(DeviceType::Wireless);
        if (!ctrl)
            ctrl = new DeviceControllItem(DeviceType::Wireless, m_netListView->viewport());
        else
            ctrl->updateView();

        ctrl->setDevices(devices);
        items << ctrl;
    }

    // 遍历当前所有的无线网卡
    auto accessPoints = [ & ] (WirelessDevice *device) {
        if (device->isEnabled())
            return device->accessPointItems();

        return QList<AccessPoints *>();
    };

    for (WirelessDevice *device : wirelessDevices) {
        WirelessControllItem *ctrl = findWirelessController(device);
        if (!ctrl)
            ctrl = new WirelessControllItem(m_netListView->viewport(), static_cast<WirelessDevice *>(device));
        ctrl->updateView();

        items << ctrl;
        if (device->isEnabled() && !NetworkController::instance()->hotspotController()->enabled(device)) {
            QList<AccessPoints *> aps = accessPoints(device);
            for (AccessPoints *ap : aps) {
                WirelessItem *apCtrl = findWirelessItem(ap, device);
                if (!apCtrl) {
                    apCtrl = new WirelessItem(m_netListView->viewport(), device, ap);
                    connect(apCtrl, &WirelessItem::sizeChanged, this, &NetworkPanel::updateSize);
                }
                apCtrl->updateView();

                items << apCtrl;
            }
            // 连接隐藏网络
            WirelessItem *apCtrl = findWirelessItem(nullptr, device);
            if (!apCtrl) {
                apCtrl = new WirelessItem(m_netListView->viewport(), device, nullptr);
                connect(apCtrl, &WirelessItem::sizeChanged, this, &NetworkPanel::updateSize);
            }
            apCtrl->updateView();
            items << apCtrl;
        }
    }

    // 存在多个有线设备的情况下，需要显示总开关
    if (wiredDevices.size() > 1) {
        DeviceControllItem *ctrl = findBaseController(DeviceType::Wired);
        if (!ctrl)
            ctrl = new DeviceControllItem(DeviceType::Wired, m_netListView->viewport());
        ctrl->updateView();

        ctrl->setDevices(devices);
        items << ctrl;
    }

    auto wiredConnections = [ & ] (WiredDevice *device) {
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

        items << ctrl;

        QList<WiredConnection *> connItems = wiredConnections(device);
        for (WiredConnection *conn : connItems) {
            WiredItem *connectionCtrl = findWiredItem(conn);
            if (!connectionCtrl)
                connectionCtrl = new WiredItem(m_netListView->viewport(), device, conn);
            connectionCtrl->updateView();

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
    updateSize();
    passwordError(QString(), QString());
}

void NetworkPanel::updateSize()
{
    QList<QStandardItem *> items;
    for (int i = 0; i < m_items.size(); i++) {
        NetItem *item = m_items[i];
        int nRow = item->standardItem()->row();
        if (nRow < 0) {
            m_model->insertRow(i, item->standardItem());
        } else if (nRow != i) {
            m_model->takeItem(nRow, 0);
            m_model->removeRow(nRow);
            m_model->insertRow(i, item->standardItem());
        }

        items << item->standardItem();
    }

    // 删除model在items中不存在的项目
    QList<int> rmRows;
    for (int i = 0; i < m_model->rowCount(); i++) {
        QStandardItem *item = m_model->item(i);
        if (!items.contains(item))
            rmRows << item->row();
    }

    // 将row按照从大到小的顺序排序，否则会出现删除错误的问题
    qSort(rmRows.begin(), rmRows.end(), [=](int &row1, int &row2) { return row1 > row2; });
    for (int row : rmRows)
        m_model->removeRow(row);

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
    getPluginState();
    updateView();
}

void NetworkPanel::onClickListView(const QModelIndex &index)
{
    NetItem *selectItem = m_items.at(index.row());
    if (selectItem != m_selectItem
        && m_items.contains(m_selectItem)) {
        WirelessItem *item = static_cast<WirelessItem *>(m_selectItem);
        item->expandWidget(WirelessItem::Hide); // 选择切换时隐藏输入框
        m_selectItem = nullptr;
    }
    NetItemType type = static_cast<NetItemType>(index.data(NetItemRole::TypeRole).toInt());
    switch (type) {
    case WirelessViewItem: {
        m_selectItem = selectItem;
        WirelessItem *item = static_cast<WirelessItem *>(selectItem);
        item->connectNetwork();
        break;
    }
    case WiredViewItem: {
        WiredItem *item = static_cast<WiredItem *>(selectItem);
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

    for (NetItem *item : m_items) {
        if (NetItemType::WirelessViewItem == item->itemType()) {
            WirelessItem *wirelessItem = static_cast<WirelessItem *>(item);
            if (wirelessItem->accessPoint()
                && (m_reconnectDev.isEmpty() || wirelessItem->wirelessDevice()->path() == m_reconnectDev)
                && (wirelessItem->accessPoint()->ssid() == m_reconnectSsid)) {
                if (item != m_selectItem
                    && m_items.contains(m_selectItem)) {
                    WirelessItem *selectItem = static_cast<WirelessItem *>(m_selectItem);
                    selectItem->expandWidget(WirelessItem::Hide); // 选择切换时隐藏输入框
                    m_selectItem = nullptr;
                }
                wirelessItem->expandPasswordInput();
                m_selectItem = wirelessItem;
                m_reconnectSsid.clear();
                m_reconnectDev.clear();
                break;
            }
        }
    }
}
// 用于绘制分割线
#define RIGHTMARGIN 13
#define DIAMETER 16

NetworkDelegate::NetworkDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
    , m_parentWidget(parent)
    , m_currentDegree(0)
    , m_refreshTimer(new QTimer(this))
{
    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        this->m_currentDegree += 14;
        m_parentWidget->update();
    });

    m_refreshTimer->setInterval(30);
}

NetworkDelegate::~NetworkDelegate()
{
    if (m_refreshTimer->isActive())
        m_refreshTimer->stop();
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
        QRect rct = option.rect;
        rct.setY(rct.top() + rct.height() - 2);
        rct.setHeight(2);
        painter->fillRect(rct, ThemeManager::instance()->lineColor());
    }
    bool isHoverItem = cantHover(index);
    QRect rect = option.rect;
    if (drawLine)
        rect.setHeight(rect.height() - 2);

    if (!isHoverItem && (option.state & QStyle::State_MouseOver)) {
        painter->fillRect(rect, ThemeManager::instance()->itemBackgroundColor());
    }
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
    NetItemType itemType = static_cast<NetItemType>(index.data(TypeRole).toInt());
    if (itemType == NetItemType::DeviceControllViewItem
        || itemType == NetItemType::WirelessControllViewItem
        || itemType == NetItemType::WiredControllViewItem)
        return true;

    NetItemType nextItemType = static_cast<NetItemType>(siblingIndex.data(TypeRole).toInt());
    return itemType != nextItemType;
}

bool NetworkDelegate::cantHover(const QModelIndex &index) const
{
    NetItemType itemType = static_cast<NetItemType>(index.data(TypeRole).toInt());
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
        QPointF(rect.left() + radius / 100.0 * 75, rect.top() + radius / 100.0 * 35)};

    painter->drawPolyline(points, 3);
}

void NetworkDelegate::drawFork(QPainter *painter, QRect &rect, QPen &pen, int radius) const
{
    painter->drawPie(rect, 0, 360 * 16);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    QPointF pointsl[2] = {
        QPointF(rect.left() + radius / 100.0 * 35, rect.top() + radius / 100.0 * 35),
        QPointF(rect.left() + radius / 100.0 * 65, rect.top() + radius / 100.0 * 65)};

    painter->drawPolyline(pointsl, 2);

    QPointF pointsr[2] = {
        QPointF(rect.left() + radius / 100.0 * 65, rect.top() + radius / 100.0 * 35),
        QPointF(rect.left() + radius / 100.0 * 35, rect.top() + radius / 100.0 * 65)};

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
        auto colors = indicatorColors.value(i);
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
        NetConnectionType connectionStatus = static_cast<NetConnectionType>(index.data(NetItemRole::ConnectionStatusRole).toInt());
        if (connectionStatus == NetConnectionType::Connected) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QRect rct = checkRect(option.rect);
            if (rct.contains(mouseEvent->pos())) {
                Q_EMIT closeClicked(index);
                return true;
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
    int left = rct.right() - RIGHTMARGIN - DIAMETER;
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
