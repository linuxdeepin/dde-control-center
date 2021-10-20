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

#include "netItem.h"
#include "constants.h"

#include <DApplicationHelper>
#include <DHiDPIHelper>
#include <DSwitchButton>
#include <DListView>
#include <dloadingindicator.h>
#include <DSpinner>

#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>
#include <networkconst.h>

#define SWITCH_WIDTH 60
#define SWITCH_HEIGHT 32

DWIDGET_USE_NAMESPACE

/**
 * 单个列表项的基类
 */
NetItem::NetItem(QWidget *parent)
    : QObject(parent)
    , m_standardItem(new DStandardItem)
    , m_parentWidget(parent)
{
    m_standardItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_standardItem->setData(NetConnectionType::UnConnected, ItemIsCheckRole);
}

NetItem::~NetItem()
{
}

DStandardItem *NetItem::standardItem()
{
    return m_standardItem;
}

/**
 * @brief baseControllItem::baseControllItem
 * 总线控制器
 */
DeviceControllItem::DeviceControllItem(const DeviceType &deviceType, QWidget *parent)
    : NetItem(parent)
    , m_deviceType(deviceType)
    , m_switcher(new DSwitchButton(parent))
{
    initItemText();
    initSwitcher();
    initConnection();
}

DeviceControllItem::~DeviceControllItem()
{
    m_switcher->deleteLater();
}

void DeviceControllItem::setDevices(const QList<NetworkDeviceBase *> &devices)
{
    m_devices.clear();
    for (NetworkDeviceBase *device : devices)
        if (m_deviceType == device->deviceType())
            m_devices << device;
}

DeviceType DeviceControllItem::deviceType()
{
    return m_deviceType;
}

void DeviceControllItem::updateView()
{
    // 更新状态显示
    bool onOrOff = false;
    for (NetworkDeviceBase *device : m_devices) {
        if (device->isEnabled()) {
            onOrOff = true;
            break;
        }
    }

    // 阻塞状态，防止多次触发
    m_switcher->blockSignals(true);
    m_switcher->setChecked(onOrOff);
    m_switcher->blockSignals(false);
}

NetItemType DeviceControllItem::itemType()
{
    return DeviceControllViewItem;
}

void DeviceControllItem::initItemText()
{
    // 创建左侧的切换文本
    standardItem()->setSizeHint(QSize(-1, 46));
    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setData(NetItemType::DeviceControllViewItem, NetItemRole::TypeRole);
    standardItem()->setFontSize(DFontSizeManager::T3);

    if (m_deviceType == DeviceType::Wireless)
        standardItem()->setText(tr("Wireless Network"));
    else
        standardItem()->setText(tr("Wired Network"));
}

void DeviceControllItem::initSwitcher()
{
    // 创建右侧的切换开关
    DViewItemAction *switchAction = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter,
           QSize(0, 0), QSize(0, 0), false);
    m_switcher->setFixedSize(SWITCH_WIDTH, SWITCH_HEIGHT);
    switchAction->setWidget(m_switcher);
    standardItem()->setActionList(Qt::RightEdge, { switchAction });
}

void DeviceControllItem::initConnection()
{
    // 创建连接的信号槽
    connect(m_switcher, &DSwitchButton::checkedChanged, this, &DeviceControllItem::onSwitchDevices);
}

void DeviceControllItem::onSwitchDevices(bool on)
{
    for (NetworkDeviceBase *device : m_devices)
        if (device->isEnabled() != on)
            device->setEnabled(on);
}

WiredControllItem::WiredControllItem(QWidget *parent, WiredDevice *device)
    : NetItem(parent)
    , m_device(device)
    , m_switcher(new DSwitchButton(parent))
{
    standardItem()->setSizeHint(QSize(-1, 46));
    standardItem()->setText(device->deviceName());
    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setData(NetItemType::WiredControllViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)), NetItemRole::DeviceDataRole);
    standardItem()->setFontSize(DFontSizeManager::T4);

    m_switcher->setFixedSize(SWITCH_WIDTH, SWITCH_HEIGHT);
    m_switcher->setChecked(m_device->isEnabled());

    DViewItemAction *switchAction = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter,
           QSize(0, 0), QSize(0, 0), false);
    switchAction->setWidget(m_switcher);
    standardItem()->setActionList(Qt::RightEdge, { switchAction });

    connect(m_switcher, &DSwitchButton::checkedChanged,
            this, &WiredControllItem::onSwitchDevices);
}

WiredControllItem::~WiredControllItem()
{
    m_switcher->deleteLater();
}

WiredDevice *WiredControllItem::device()
{
    return m_device;
}

void WiredControllItem::updateView()
{
    standardItem()->setText(m_device->deviceName());

    m_switcher->blockSignals(true);
    m_switcher->setChecked(m_device->isEnabled());
    m_switcher->blockSignals(false);
}

NetItemType WiredControllItem::itemType()
{
    return WiredControllViewItem;
}

void WiredControllItem::onSwitchDevices(bool on)
{
    if (m_device->isEnabled() != on)
        m_device->setEnabled(on);
}

QString WirelessControllItem::iconFile()
{
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        return QString(":/wireless/resources/wireless/refresh_dark.svg");

    return QString(":/wireless/resources/wireless/refresh.svg");
}

WirelessControllItem::WirelessControllItem(QWidget *parent, WirelessDevice *device)
    : NetItem(parent)
    , m_device(device)
    , m_widget(new QWidget(parent))
    , m_switcher(new DSwitchButton(m_widget))
    , m_loadingIndicator(new DLoadingIndicator(m_widget))
{
    QHBoxLayout *layout = new QHBoxLayout(m_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    m_widget->setFixedSize(SWITCH_WIDTH + 50, SWITCH_HEIGHT);
    m_widget->setLayout(layout);

    m_switcher->setFixedSize(SWITCH_WIDTH, SWITCH_HEIGHT);
    m_switcher->setChecked(device->isEnabled());

    QPixmap pixmap = DHiDPIHelper::loadNxPixmap(":/wireless/resources/wireless/refresh.svg");
    m_loadingIndicator->setLoading(false);
    m_loadingIndicator->setSmooth(true);
    m_loadingIndicator->setAniDuration(1000);
    m_loadingIndicator->setAniEasingCurve(QEasingCurve::InOutCirc);
    m_loadingIndicator->installEventFilter(this);
    m_loadingIndicator->setFixedSize(pixmap.size() / m_widget->devicePixelRatioF());
    m_loadingIndicator->viewport()->setAutoFillBackground(false);
    m_loadingIndicator->setFrameShape(QFrame::NoFrame);
    m_loadingIndicator->installEventFilter(this);

    layout->addWidget(m_loadingIndicator);
    layout->addStretch();
    layout->addWidget(m_switcher);

    DViewItemAction *switchAction = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter, QSize(0, 0), QSize(0, 0), false);
    switchAction->setWidget(m_widget);

    standardItem()->setSizeHint(QSize(-1, 46));
    standardItem()->setText(device->deviceName());
    standardItem()->setActionList(Qt::RightEdge, { switchAction });
    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setData(NetItemType::WirelessControllViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)), NetItemRole::DeviceDataRole);
    standardItem()->setFontSize(DFontSizeManager::T4);

    connect(m_switcher, &DSwitchButton::checkedChanged, this, &WirelessControllItem::onSwitchDevices);
}

WirelessControllItem::~WirelessControllItem()
{
    m_widget->deleteLater();
}

WirelessDevice *WirelessControllItem::device()
{
    return m_device;
}

void WirelessControllItem::updateView()
{
    standardItem()->setText(m_device->deviceName());

    m_switcher->blockSignals(true);
    m_switcher->setChecked(m_device->isEnabled());
    m_switcher->blockSignals(false);

    // 刷新按钮图标
    QPixmap pix = DHiDPIHelper::loadNxPixmap(iconFile());
    m_loadingIndicator->setImageSource(pix);
}

NetItemType WirelessControllItem::itemType()
{
    return WirelessControllViewItem;
}

bool WirelessControllItem::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_loadingIndicator) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_device->scanNetwork();
            if (!m_loadingIndicator->loading()) {
                m_loadingIndicator->setLoading(true);
                QTimer::singleShot(1000, this, [ = ] {
                   m_loadingIndicator->setLoading(false);
                });
            }
        }
    }
    return QObject::eventFilter(object, event);
}

void WirelessControllItem::onSwitchDevices(bool on)
{
    if (m_device->isEnabled() != on)
        m_device->setEnabled(on);
}

WiredItem::WiredItem(QWidget *parent, WiredDevice *device, WiredConnection *connection)
    : NetItem(parent)
    , m_connection(connection)
    , m_device(device)
{
    initUi();
}

WiredItem::~WiredItem()
{
}

WiredConnection *WiredItem::connection()
{
    return m_connection;
}

void WiredItem::updateView()
{
    standardItem()->setText(m_connection->connection()->id());
    switch (m_connection->status()) {
    case ConnectionStatus::Activating:
        standardItem()->setData(NetConnectionType::Connecting, ItemIsCheckRole);
        break;
    case ConnectionStatus::Activated:
        standardItem()->setData(NetConnectionType::Connected, ItemIsCheckRole);
        break;
    default:
        standardItem()->setData(NetConnectionType::UnConnected, ItemIsCheckRole);
        break;
    }
}

NetItemType WiredItem::itemType()
{
    return WiredViewItem;
}

void WiredItem::initUi()
{
    standardItem()->setSizeHint(QSize(-1, 36));
    standardItem()->setText(m_connection->connection()->id());

    // 占位的
    DViewItemAction *emptyAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter,
                                                         QSize(14, 20), QSize(14, 20), false);

    standardItem()->setActionList(Qt::LeftEdge, { emptyAction });
    updateView();

    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setFontSize(DFontSizeManager::T6);
    standardItem()->setData(NetItemType::WiredViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_connection)), NetItemRole::DataRole);
}

WirelessItem::WirelessItem(QWidget *parent, WirelessDevice *device, AccessPoints *ap)
    : NetItem(parent)
    , m_accessPoint(ap)
    , m_device(device)
{
    initUi();
}

WirelessItem::~WirelessItem()
{
}

const AccessPoints *WirelessItem::accessPoint()
{
    return m_accessPoint;
}

void WirelessItem::updateView()
{
    updateSrcirityIcon();
    updateWifiIcon();
    updateConnectionStatus();
}

NetItemType WirelessItem::itemType()
{
    return WirelessViewItem;
}

QString WirelessItem::getStrengthStateString(int strength)
{
    if (5 >= strength)
        return "0";

    if (30 >= strength)
        return "20";

    if (55 >= strength)
        return "40";

    if (65 >= strength)
        return "60";

    return "80";
}

void WirelessItem::initUi()
{
    // 左侧的加密图标
    m_securityAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter, QSize(20, 20), QSize(20, 20), false);
    updateSrcirityIcon();

    // 绘制WiFi图标
    m_wifiLabel = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter, QSize(20, 20), QSize(20, 20), false);
    updateWifiIcon();

    standardItem()->setSizeHint(QSize(-1, 36));
    standardItem()->setActionList(Qt::LeftEdge, { m_securityAction, m_wifiLabel });

    standardItem()->setText(m_accessPoint->ssid());

    // 绘制右侧的连接图标
    standardItem()->setFlags(Qt::ItemIsEnabled);
    updateConnectionStatus();

    standardItem()->setData(NetItemType::WirelessViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)) ,NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_accessPoint)), NetItemRole::DataRole);
    standardItem()->setBackground(Qt::transparent);
    standardItem()->setFontSize(DFontSizeManager::T6);
}

void WirelessItem::updateSrcirityIcon()
{
    if (m_accessPoint->secured()) {
        QString srcirityIcon;
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
            srcirityIcon = ":/wireless/resources/wireless/security_dark.svg";
        else
            srcirityIcon = ":/wireless/resources/wireless/security.svg";
        // 更新加密图标
        m_securityAction->setIcon(QIcon(srcirityIcon));
    } else {
        m_securityAction->setIcon(QIcon());
    }
}

void WirelessItem::updateWifiIcon()
{
    QString icon;
    QString strength = getStrengthStateString(m_accessPoint->strength());
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        icon = QString(":/wireless/resources/wireless/wireless-%1-symbolic-dark.svg").arg(strength);
    else
        icon = QString(":/wireless/resources/wireless/wireless-%1-symbolic.svg").arg(strength);

    m_wifiLabel->setIcon(QIcon(icon));
}

void WirelessItem::updateConnectionStatus()
{
    switch (m_accessPoint->status()) {
    case ConnectionStatus::Activating:
        standardItem()->setData(NetConnectionType::Connecting, ItemIsCheckRole);
        break;
    case ConnectionStatus::Activated:
        standardItem()->setData(NetConnectionType::Connected, ItemIsCheckRole);
        break;
    default:
        standardItem()->setData(NetConnectionType::UnConnected, ItemIsCheckRole);
        break;
    }
}
