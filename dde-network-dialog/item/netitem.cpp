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

#include "netitem.h"
#include "../widgets/statebutton.h"
#include "wirelessconnect.h"
#include "../localclient.h"
#include "../thememanager.h"

#include <DApplicationHelper>
#include <DHiDPIHelper>
#include <DListView>
#include <dloadingindicator.h>
#include <DSpinner>
#include <DLineEdit>
#include <DToolButton>
#include <DPasswordEdit>
#include <DSuggestButton>

#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>
#include <networkconst.h>
#include <networkcontroller.h>

#define SWITCH_WIDTH 50
#define SWITCH_HEIGHT 24

DWIDGET_USE_NAMESPACE

#define MAXTEXTWIDTH 180
/**
 * 单个列表项的基类
 */
NetItem::NetItem(QWidget *parent)
    : QObject(parent)
    , m_standardItem(new DStandardItem)
    , m_parentWidget(parent)
{
    m_standardItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_standardItem->setData(NetConnectionType::UnConnected, ConnectionStatusRole);
    m_standardItem->setBackground(Qt::transparent);
    m_standardItem->setTextColorRole(DPalette::BrightText);
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
{
    initItemText();
}

DeviceControllItem::~DeviceControllItem()
{
}

void DeviceControllItem::setDevices(const QList<NetworkDeviceBase *> &devices)
{
    m_devices.clear();
    for (NetworkDeviceBase *device : devices) {
        if (m_deviceType == device->deviceType()) {
            connect(device, &NetworkDeviceBase::destroyed, this, [ this, device ] {
                m_devices.removeOne(device);
                standardItem()->setData(QVariant::fromValue<QList<NetworkDeviceBase *>>(m_devices), DeviceDataRole);
            });
            m_devices << device;
        }
    }

    updateView();
}

DeviceType DeviceControllItem::deviceType()
{
    return m_deviceType;
}

void DeviceControllItem::updateView()
{
    // 更新状态显示
    standardItem()->setData(QVariant::fromValue<QList<NetworkDeviceBase *>>(m_devices), DeviceDataRole);
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
    standardItem()->setData(QVariant::fromValue(m_deviceType), NetItemRole::DeviceTypeRole);
    standardItem()->setFontSize(DFontSizeManager::T4);

    if (m_deviceType == DeviceType::Wireless)
        standardItem()->setText(tr("Wireless Network"));
    else
        standardItem()->setText(tr("Wired Network"));
}

WiredControllItem::WiredControllItem(QWidget *parent, WiredDevice *device)
    : NetItem(parent)
    , m_device(device)
{
    standardItem()->setSizeHint(QSize(-1, 46));
    standardItem()->setText(device->deviceName());
    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setData(NetItemType::WiredControllViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(DeviceType::Wired), NetItemRole::DeviceTypeRole);
    standardItem()->setData(QVariant::fromValue(m_device), NetItemRole::DeviceDataRole);
    standardItem()->setFontSize(DFontSizeManager::T5);
    connect(m_device, &WiredDevice::destroyed, this, [ this ] {
        m_device = nullptr;
        standardItem()->setData(0, NetItemRole::DeviceDataRole);
    });
}

WiredControllItem::~WiredControllItem()
{
}

WiredDevice *WiredControllItem::device()
{
    return m_device;
}

void WiredControllItem::updateView()
{
    standardItem()->setText(m_device->deviceName());
}

NetItemType WiredControllItem::itemType()
{
    return WiredControllViewItem;
}

QString WirelessControllItem::iconFile()
{
    return ThemeManager::instance()->getIcon("wireless/refresh");
}

WirelessControllItem::WirelessControllItem(QWidget *parent, WirelessDevice *device)
    : NetItem(parent)
    , m_device(device)
{
    standardItem()->setSizeHint(QSize(-1, 46));
    standardItem()->setText(device->deviceName());
    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setData(NetItemType::WirelessControllViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(m_device), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(DeviceType::Wireless), NetItemRole::DeviceTypeRole);
    standardItem()->setFontSize(DFontSizeManager::T5);
    connect(m_device, &WirelessDevice::destroyed, this, [ this ] {
        m_device = nullptr;
        standardItem()->setData(0, NetItemRole::DeviceDataRole);
    });
}

WirelessControllItem::~WirelessControllItem()
{
}

WirelessDevice *WirelessControllItem::device()
{
    return m_device;
}

void WirelessControllItem::updateView()
{
    standardItem()->setText(m_device->deviceName());
}

NetItemType WirelessControllItem::itemType()
{
    return WirelessControllViewItem;
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

QString WiredItem::symbolicIcon(const bool &connected) const
{
    QString icon = connected ? QString("wired/network-wired-symbolic") : QString("wired/network-none-symbolic");
    return ThemeManager::instance()->getIcon(icon);
}

void WiredItem::updateView()
{
    // 更新显示的文本
    QFontMetrics ftm(standardItem()->font());
    QString displayText = ftm.elidedText(m_connection->connection()->id(), Qt::TextElideMode::ElideRight, MAXTEXTWIDTH);
    standardItem()->setText(displayText);
    // 更新当前的连接状态
    QString connectionIconFile;
    switch (m_connection->status()) {
    case ConnectionStatus::Activating:
        standardItem()->setData(NetConnectionType::Connecting, ConnectionStatusRole);
        connectionIconFile = symbolicIcon(false);
        break;
    case ConnectionStatus::Activated:
        standardItem()->setData(NetConnectionType::Connected, ConnectionStatusRole);
        connectionIconFile = symbolicIcon(true);
        break;
    default:
        standardItem()->setData(NetConnectionType::UnConnected, ConnectionStatusRole);
        connectionIconFile = symbolicIcon(false);
        break;
    }

    // 设置左侧的连接图标
    m_connectionIconAction->setIcon(QIcon(connectionIconFile));
}

NetItemType WiredItem::itemType()
{
    return WiredViewItem;
}

void WiredItem::initUi()
{
    standardItem()->setSizeHint(QSize(-1, 36));

    // 占位的
    DViewItemAction *emptyAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter,
                                                       QSize(20, 20), QSize(20, 20), false);

    m_connectionIconAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter,
                                                 QSize(20, 20), QSize(20, 20), false);

    standardItem()->setActionList(Qt::LeftEdge, { emptyAction, m_connectionIconAction });
    updateView();

    standardItem()->setFlags(Qt::ItemIsEnabled);
    standardItem()->setFontSize(DFontSizeManager::T6);
    standardItem()->setData(NetItemType::WiredViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(m_device), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_connection)), NetItemRole::DataRole);
    standardItem()->setData(QVariant::fromValue(DeviceType::Wired), NetItemRole::DeviceTypeRole);
    connect(m_device, &WiredDevice::destroyed, this, [ this ] {
        m_device = nullptr;
        standardItem()->setData(0, NetItemRole::DeviceDataRole);
    });
}

void WiredItem::connectNetwork()
{
    if (m_connection && !m_connection->connected()) {
        m_device->connectNetwork(m_connection);
    }
}

WirelessItem::WirelessItem(QWidget *parent, WirelessDevice *device, AccessPoints *ap)
    : NetItem(parent)
    , m_accessPoint(ap)
    , m_device(device)
    , m_securityAction(nullptr)
    , m_wifiLabel(nullptr)
    , m_connectionAction(nullptr)
    , m_loadingStat(nullptr)
    , m_connectionWidget(nullptr)
    , m_connIcon(nullptr)
    , m_stackWidget(nullptr)
    , m_expandItem(nullptr)
    , m_topItem(nullptr)
    , m_passwdEdit(nullptr)
    , m_ssidEdit(nullptr)
    , m_wirelessConnect(new WirelessConnect(this, device, ap))
    , m_connectButton(nullptr)

{
    initUi(parent);
    initConnection();
    if (m_accessPoint) {
        m_wirelessConnect->setSsid(ap->ssid());
        QFontMetrics ftm(standardItem()->font());
        QString displayText = ftm.elidedText(m_accessPoint->ssid(), Qt::TextElideMode::ElideRight, MAXTEXTWIDTH);
        standardItem()->setText(displayText);
    } else {
        m_wifiLabel->setVisible(false);
        m_securityAction->setVisible(false);
        standardItem()->setText(tr("Connect to hidden network"));
    }
}

WirelessItem::~WirelessItem()
{
    m_stackWidget->setParent(nullptr);
    m_stackWidget->deleteLater();
}

const AccessPoints *WirelessItem::accessPoint()
{
    return m_accessPoint;
}

const WirelessDevice *WirelessItem::wirelessDevice()
{
    return m_device;
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

void WirelessItem::initUi(QWidget *parent)
{
    m_expandItem = new DViewItemAction(Qt::AlignBottom, QSize(PANELWIDTH, 20), QSize(PANELWIDTH, 20), false);
    m_stackWidget = new DStackedWidget(parent);
    // 初始化展开输入控件
    initExpandUi();
    m_expandItem->setWidget(m_stackWidget);
    standardItem()->setActionList(Qt::BottomEdge, { m_expandItem });
    m_expandItem->setVisible(false);
    // 左侧的加密图标
    m_securityAction = new DViewItemAction(Qt::AlignLeft , QSize(20, 35), QSize(20, 35), false);
    updateSrcirityIcon();
    // 绘制WiFi图标
    m_wifiLabel = new DViewItemAction(Qt::AlignLeft , QSize(20, 35), QSize(20, 35), false);
    updateWifiIcon();

    standardItem()->setSizeHint(QSize(-1, 36));
    standardItem()->setActionList(Qt::LeftEdge, { m_securityAction, m_wifiLabel });

    m_topItem = new DViewItemAction(Qt::AlignTop, QSize(-1, 1), QSize(-1, 1), false);
    standardItem()->setActionList(Qt::TopEdge, { m_topItem });
    m_topItem->setVisible(false);
    // 绘制右侧的连接图标
    standardItem()->setFlags(Qt::ItemIsEnabled);
    updateConnectionStatus();
    if (m_accessPoint) {
        standardItem()->setData(NetItemType::WirelessViewItem, NetItemRole::TypeRole);
    } else {
        standardItem()->setData(NetItemType::WirelessHiddenViewItem, NetItemRole::TypeRole);
    }
    standardItem()->setData(QVariant::fromValue(m_device), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(DeviceType::Wireless), NetItemRole::DeviceTypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_accessPoint)), NetItemRole::DataRole);
    standardItem()->setFontSize(DFontSizeManager::T6);
    connect(m_device, &WirelessDevice::destroyed, this, [ this ] {
        m_device = nullptr;
        standardItem()->setData(0, NetItemRole::DeviceDataRole);
    });
}

void WirelessItem::initConnection()
{
    connect(m_wirelessConnect, &WirelessConnect::passwordError, this, &WirelessItem::onInputPassword);
}

void WirelessItem::updateSrcirityIcon()
{
    if (m_accessPoint && m_accessPoint->secured()) {
        QString srcirityIcon = ThemeManager::instance()->getIcon("wireless/security");
        // 更新加密图标
        m_securityAction->setIcon(QIcon(srcirityIcon));
    } else {
        m_securityAction->setIcon(QIcon());
    }
}

void WirelessItem::updateWifiIcon()
{
    if (!m_accessPoint)
        return;

    bool isWlan6 = (m_accessPoint->type() == AccessPoints::WlanType::wlan6);
    QString strength = getStrengthStateString(m_accessPoint->strength());
    QString iconRes = QString(isWlan6 ? QString("wireless6/wireless6-%1-symbolic")
                                      : QString("wireless/wireless-%1-symbolic")).arg(strength);
    QString icon = ThemeManager::instance()->getIcon(iconRes);
    m_wifiLabel->setIcon(QIcon(icon));
}

void WirelessItem::updateConnectionStatus()
{
    if (!m_accessPoint)
        return;

    switch (m_accessPoint->status()) {
    case ConnectionStatus::Activating:
        standardItem()->setData(NetConnectionType::Connecting, ConnectionStatusRole);
        break;
    case ConnectionStatus::Activated:
        standardItem()->setData(NetConnectionType::Connected, ConnectionStatusRole);
        expandWidget(ExpandWidget::Hide);
        break;
    default:
        standardItem()->setData(NetConnectionType::UnConnected, ConnectionStatusRole);
        break;
    }
}

void WirelessItem::onConnection()
{
    if (m_device->activeAccessPoints() == m_accessPoint)
        m_device->disconnectNetwork();
}

void WirelessItem::expandWidget(ExpandWidget type, bool autoDisconnect)
{
    switch (type) {
    case ExpandWidget::Hide:
        m_expandItem->setVisible(false);
        m_topItem->setVisible(false);
        standardItem()->setSizeHint(QSize(-1, 36));
        if (m_accessPoint) {
            LocalClient::instance()->changePassword(m_accessPoint->ssid(), QString(), false);
            if(autoDisconnect && m_accessPoint->status() == ConnectionStatus::Activating) {
                m_device->disconnectNetwork();
            }
        }
        break;
    case ExpandWidget::ShowSSID:
        m_expandItem->setVisible(true);
        m_topItem->setVisible(true);
        standardItem()->setSizeHint(QSize(-1, 130));
        m_stackWidget->setCurrentIndex(type);
        m_ssidEdit->lineEdit()->setFocus();
        break;
    case ExpandWidget::ShowPassword:
        m_expandItem->setVisible(true);
        m_topItem->setVisible(true);
        standardItem()->setSizeHint(QSize(-1, 130));
        m_stackWidget->setCurrentIndex(type);
        m_passwdEdit->lineEdit()->setFocus();
        checkInputValid();
        m_passwdEdit->setAlert(!m_passwdEdit->text().isEmpty());
        break;
    }
    emit sizeChanged();
}

void WirelessItem::createPasswordEdit()
{
    //　密码输入窗
    DWidget *passwdWidget = new DWidget(m_stackWidget);
    m_passwdEdit = new DPasswordEdit(passwdWidget);
    m_passwdEdit->lineEdit()->setPlaceholderText(tr("Password"));
    m_passwdEdit->lineEdit()->setMaxLength(256);
    m_passwdEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_passwdEdit->setFixedHeight(36);
    m_passwdEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DPushButton *cancelButtion = new DPushButton(tr("Cancel", "button"), passwdWidget); // 取消
    m_connectButton = new DSuggestButton(tr("Connect", "button"), passwdWidget);        // 连接
    cancelButtion->setFixedHeight(36);
    cancelButtion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_connectButton->setFixedHeight(36);
    m_connectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *line2 = new QHBoxLayout;
    line2->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout(passwdWidget);
    layout->setContentsMargins(0, 0, 10, 10);
    line2->addWidget(cancelButtion);
    line2->addSpacing(3);
    line2->addWidget(m_connectButton);

    layout->addWidget(m_passwdEdit);
    layout->addSpacing(10);
    layout->addLayout(line2);
    layout->addSpacing(7);

    passwdWidget->setLayout(layout);
    m_stackWidget->addWidget(passwdWidget);

    connect(cancelButtion, &DPushButton::clicked, this, &WirelessItem::onCancel);
    connect(m_connectButton, &DPushButton::clicked, this, &WirelessItem::onConnectNetwork);
    connect(m_passwdEdit->lineEdit(), &QLineEdit::returnPressed, this, &WirelessItem::onConnectNetwork);
    connect(m_passwdEdit->lineEdit(), &QLineEdit::textChanged, this, &WirelessItem::checkInputValid);
    ThemeManager::instance()->updateInputStyle(m_passwdEdit);
}

void WirelessItem::createSsidEdit()
{
    // ssid输入窗
    DWidget *ssidWidget = new DWidget(m_stackWidget);
    m_ssidEdit = new DLineEdit(ssidWidget);
    m_ssidEdit->setPlaceholderText(tr("Name (SSID)"));
    m_ssidEdit->lineEdit()->setMaxLength(256);
    m_ssidEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_ssidEdit->setFixedHeight(36);
    m_ssidEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DPushButton *cancelButtion = new DPushButton(tr("Cancel", "button"), ssidWidget);     // 取消
    DPushButton *connectButton = new DSuggestButton(tr("Connect", "button"), ssidWidget); // 连接
    cancelButtion->setFixedHeight(36);
    cancelButtion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connectButton->setFixedHeight(36);
    connectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *line2 = new QHBoxLayout;
    line2->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout(ssidWidget);
    layout->setContentsMargins(0, 0, 10, 10);
    line2->addWidget(cancelButtion);
    line2->addSpacing(3);
    line2->addWidget(connectButton);

    layout->addWidget(m_ssidEdit);
    layout->addSpacing(10);
    layout->addLayout(line2);
    layout->addSpacing(7);

    ssidWidget->setLayout(layout);
    m_stackWidget->addWidget(ssidWidget);

    connect(cancelButtion, &DPushButton::clicked, this, &WirelessItem::onCancel);
    connect(connectButton, &DPushButton::clicked, this, &WirelessItem::onConnectHidden);
    connect(m_ssidEdit->lineEdit(), &QLineEdit::returnPressed, this, &WirelessItem::onConnectHidden);
    ThemeManager::instance()->updateInputStyle(m_ssidEdit);
}

void WirelessItem::initExpandUi()
{
    createPasswordEdit();
    createSsidEdit();
    m_stackWidget->setContentsMargins(0, 0, 0, 0);
    m_stackWidget->setFixedSize(PANELWIDTH - 10, 92);
}

void WirelessItem::connectNetwork()
{
    if (NetConnectionType::UnConnected == standardItem()->data(ConnectionStatusRole)) {
        // 密码框未显示前尝试直接连接
        if (!m_expandItem->isVisible()) {
            if (m_accessPoint) {
                m_wirelessConnect->connectNetwork();
            } else {
                expandWidget(ExpandWidget::ShowSSID);
            }
        }
    }
}

void WirelessItem::onConnectNetwork()
{
    QString password = m_passwdEdit->text();
    // 输入无效在checkInputValid里已判断
    if (m_wirelessConnect->passwordIsValid(password)) {
        if (m_accessPoint) {
            if (LocalClient::instance()->changePassword(m_accessPoint->ssid(), password, true)) {
                expandWidget(ExpandWidget::Hide, false);
                return;
            }
        }
        m_wirelessConnect->connectNetworkPassword(m_passwdEdit->text());
        expandWidget(ExpandWidget::Hide, false);
    }
}

void WirelessItem::onInputPassword(const QString oldPassword)
{
    m_passwdEdit->setText(oldPassword);
    expandWidget(ExpandWidget::ShowPassword);
}

void WirelessItem::expandPasswordInput()
{
    m_wirelessConnect->getoldPassword();
}

void WirelessItem::onAirplaneModeChanged(bool airplaneModeEnabled)
{
    m_stackWidget->setDisabled(airplaneModeEnabled);
}

void WirelessItem::onConnectHidden()
{
    QString ssid = m_ssidEdit->text();
    if (!ssid.isEmpty()) {
        expandWidget(ExpandWidget::Hide, false);
        m_wirelessConnect->setSsid(m_ssidEdit->text());
        m_wirelessConnect->connectNetwork();
    }
}

void WirelessItem::checkInputValid()
{
    bool isValid = m_wirelessConnect->passwordIsValid(m_passwdEdit->text());

    m_passwdEdit->setAlert(false);
    m_connectButton->setEnabled(isValid);
}

bool WirelessItem::expandVisible()
{
    return m_expandItem->isVisible();
}

void WirelessItem::onCancel()
{
    if (m_accessPoint && m_accessPoint->status() == ConnectionStatus::Activating) {
        m_device->disconnectNetwork();
    }
    this->expandWidget(ExpandWidget::Hide);
}
