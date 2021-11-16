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
#include "constants.h"
#include "../widgets/statebutton.h"
#include "wirelessconnect.h"
#include "../localserver.h"
#include "../thememanager.h"

#include <DApplicationHelper>
#include <DHiDPIHelper>
#include <DSwitchButton>
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

#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>
#include <networkconst.h>
#include <networkcontroller.h>
#include <hotspotcontroller.h>

#define SWITCH_WIDTH 50
#define SWITCH_HEIGHT 24

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
    m_standardItem->setData(NetConnectionType::UnConnected, ConnectionStatusRole);
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

    updateView();
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
    standardItem()->setFontSize(DFontSizeManager::T4);

    if (m_deviceType == DeviceType::Wireless)
        standardItem()->setText(tr("Wireless Network"));
    else
        standardItem()->setText(tr("Wired Network"));
}

void DeviceControllItem::initSwitcher()
{
    // 创建右侧的切换开关
    DViewItemAction *switchAction = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter, QSize(0, 0), QSize(0, 0), false);
    m_switcher->setSizeIncrement(SWITCH_WIDTH, SWITCH_HEIGHT);
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
    standardItem()->setFontSize(DFontSizeManager::T5);

    m_switcher->setSizeIncrement(SWITCH_WIDTH, SWITCH_HEIGHT);
    m_switcher->setChecked(m_device->isEnabled());

    DViewItemAction *switchAction = new DViewItemAction(Qt::AlignRight | Qt::AlignVCenter, QSize(0, 0), QSize(0, 0), false);
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
    return ThemeManager::instance()->getIcon("wireless/refresh");
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
    m_widget->setFixedSize(SWITCH_WIDTH + 30, 46);
    m_widget->setLayout(layout);

    m_switcher->setSizeIncrement(SWITCH_WIDTH, SWITCH_HEIGHT);
    m_switcher->setChecked(device->isEnabled());

    QPixmap pixmap = DHiDPIHelper::loadNxPixmap(ThemeManager::instance()->getIcon("wireless/refresh"));
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
    standardItem()->setFontSize(DFontSizeManager::T5);

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
    m_loadingIndicator->setVisible(m_device->isEnabled() && !NetworkController::instance()->hotspotController()->enabled(m_device));
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

QString WiredItem::symbolicIcon(const bool &connected) const
{
    QString icon = connected ? QString("wired/network-wired-symbolic") : QString("wired/network-none-symbolic");
    return ThemeManager::instance()->getIcon(icon);
}

void WiredItem::updateView()
{
    // 更新显示的文本
    standardItem()->setText(m_connection->connection()->id());
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
    standardItem()->setText(m_connection->connection()->id());

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
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_connection)), NetItemRole::DataRole);
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
    , m_wirelessConnect(new WirelessConnect(this, device, ap))
{
    initUi(parent);
    initConnection();
    if (m_accessPoint) {
        m_wirelessConnect->setSsid(ap->ssid());
        standardItem()->setText(m_accessPoint->ssid());
    } else {
        m_wifiLabel->setVisible(false);
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
    m_expandItem = new DViewItemAction(Qt::AlignBottom, QSize(PANELWIDTH - 10, 20), QSize(PANELWIDTH - 10, 20), false);
    m_stackWidget = new DStackedWidget(parent);
    // 初始化展开输入控件
    initExpandUi();
    m_expandItem->setWidget(m_stackWidget);
    m_stackWidget->adjustSize();
    m_stackWidget->setFixedSize(PANELWIDTH - 10, 80);
    standardItem()->setActionList(Qt::BottomEdge, { m_expandItem });
    m_expandItem->setVisible(false);
    // 左侧的加密图标
    m_securityAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignTop, QSize(20, 35), QSize(20, 35), false);
    updateSrcirityIcon();
    // 绘制WiFi图标
    m_wifiLabel = new DViewItemAction(Qt::AlignLeft | Qt::AlignTop, QSize(20, 35), QSize(20, 35), false);
    updateWifiIcon();

    standardItem()->setSizeHint(QSize(-1, 36));
    standardItem()->setActionList(Qt::LeftEdge, { m_securityAction, m_wifiLabel });

    // 绘制右侧的连接图标
    standardItem()->setFlags(Qt::ItemIsEnabled);
    updateConnectionStatus();

    standardItem()->setData(NetItemType::WirelessViewItem, NetItemRole::TypeRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_device)), NetItemRole::DeviceDataRole);
    standardItem()->setData(QVariant::fromValue(static_cast<void *>(m_accessPoint)), NetItemRole::DataRole);
    standardItem()->setBackground(Qt::transparent);
    standardItem()->setFontSize(DFontSizeManager::T6);
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

    QString strength = getStrengthStateString(m_accessPoint->strength());
    QString icon = ThemeManager::instance()->getIcon("wireless/wireless-%1-symbolic").arg(strength);
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

void WirelessItem::expandWidget(ExpandWidget type)
{
    switch (type) {
    case ExpandWidget::Hide:
        m_expandItem->setVisible(false);
        standardItem()->setSizeHint(QSize(-1, 36));
        if (m_accessPoint) {
            LocalServer::instance()->changePassword(m_device->path() + m_accessPoint->path(), QString(), false);
        }
        break;
    case ExpandWidget::ShowSSID:
        m_expandItem->setVisible(true);
        standardItem()->setSizeHint(QSize(-1, 126));
        m_stackWidget->setCurrentIndex(type);
        m_ssidEdit->lineEdit()->setFocus();
        break;
    case ExpandWidget::ShowPassword:
        m_expandItem->setVisible(true);
        standardItem()->setSizeHint(QSize(-1, 126));
        m_stackWidget->setCurrentIndex(type);
        m_passwdEdit->lineEdit()->setFocus();
        if (!m_passwdEdit->lineEdit()->text().isEmpty()) {
            m_passwdEdit->showAlertMessage(tr("Wrong password"));
        }
        checkInputValid();
        break;
    default:
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
    DPushButton *cancelButtion = new DPushButton(tr("Cancel", "button"), passwdWidget); // 取消
    m_connectButton = new DSuggestButton(tr("Connect", "button"), passwdWidget); // 连接

    QHBoxLayout *line2 = new QHBoxLayout;

    QVBoxLayout *layout = new QVBoxLayout;
    line2->setMargin(0);
    line2->setSpacing(0);
    line2->addWidget(cancelButtion);
    line2->addSpacing(10);
    line2->addWidget(m_connectButton);
    layout->addWidget(m_passwdEdit);
    layout->addLayout(line2);
    passwdWidget->setLayout(layout);
    m_stackWidget->addWidget(passwdWidget);

    connect(cancelButtion, &DPushButton::clicked, this, [ this ]() { this->expandWidget(ExpandWidget::Hide); });
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

    DPushButton *cancelButtion = new DPushButton(tr("Cancel", "button"), ssidWidget); // 取消
    DPushButton *connectButton = new DSuggestButton(tr("Connect", "button"), ssidWidget); // 连接

    QHBoxLayout *line2 = new QHBoxLayout;

    QVBoxLayout *layout = new QVBoxLayout;
    line2->setMargin(0);
    line2->setSpacing(0);
    line2->addWidget(cancelButtion);
    line2->addSpacing(10);
    line2->addWidget(connectButton);
    layout->addWidget(m_ssidEdit);
    layout->addLayout(line2);
    ssidWidget->setLayout(layout);
    m_stackWidget->addWidget(ssidWidget);

    connect(cancelButtion, &DPushButton::clicked, this, [ this ]() { this->expandWidget(ExpandWidget::Hide); });
    connect(connectButton, &DPushButton::clicked, this, &WirelessItem::onConnectHidden);
    connect(m_ssidEdit->lineEdit(), &QLineEdit::returnPressed, this, &WirelessItem::onConnectHidden);
    ThemeManager::instance()->updateInputStyle(m_ssidEdit);
}

void WirelessItem::initExpandUi()
{
    createPasswordEdit();
    createSsidEdit();
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
            if (LocalServer::instance()->changePassword(m_device->path() + m_accessPoint->path(), password, true)) {
                expandWidget(ExpandWidget::Hide);
                return;
            }
        }
        m_wirelessConnect->connectNetworkPassword(m_passwdEdit->text());
        expandWidget(ExpandWidget::Hide);
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

void WirelessItem::onConnectHidden()
{
    QString ssid = m_ssidEdit->text();
    if (!ssid.isEmpty()) {
        expandWidget(ExpandWidget::Hide);
        m_wirelessConnect->setSsid(m_ssidEdit->text());
        m_wirelessConnect->connectNetwork();
    }
}

void WirelessItem::checkInputValid()
{
    bool isValid = m_wirelessConnect->passwordIsValid(m_passwdEdit->text());

    m_passwdEdit->setAlert(!isValid);
    m_connectButton->setEnabled(isValid);
}
