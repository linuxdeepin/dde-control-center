// SPDX-FileCopyrightText: 2016 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothapplet.h"

#include "adapter.h"
#include "adaptersmanager.h"
#include "bluetoothadapteritem.h"
#include "bluetoothconstants.h"
#include "device.h"
#include "horizontalseparator.h"
#include "jumpsettingbutton.h"
#include "constants.h"
#include "plugins-logging-category.h"
#include "touchscrollfilter.h"

#include "xdgactivation.h"

#include <DIconButton>
#include <DLabel>
#include <QProcess>
#include <DListView>
#include <DScrollArea>
#include <DSwitchButton>
#include <DTipLabel>

#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QScroller>
#include <QString>
#include <QToolButton>

SettingLabel::SettingLabel(QString text, QWidget *parent)
    : QWidget(parent)
    , m_label(new DLabel(text, this))
    , m_layout(new QHBoxLayout(this))
{
    setAccessibleName("BluetoothSettingLabel");
    setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(10, 0, 0, 0);
    m_layout->addWidget(m_label, 0, Qt::AlignLeft | Qt::AlignHCenter);
    m_layout->addStretch();

    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Window, Qt::transparent);
    this->setPalette(p);

    m_label->setForegroundRole(QPalette::BrightText);
    m_label->setElideMode(Qt::ElideRight);
}

void SettingLabel::addButton(QWidget *button, int space)
{
    m_layout->addSpacing(4);
    m_layout->addWidget(button, 0, Qt::AlignRight | Qt::AlignHCenter);
    m_layout->addSpacing(space);
}

void SettingLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        Q_EMIT clicked();
        return;
    }

    return QWidget::mousePressEvent(ev);
}

void SettingLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 0, 0);

    return QWidget::paintEvent(event);
}

BluetoothApplet::BluetoothApplet(AdaptersManager *adapterManager, QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_contentWidget(new QWidget(this))
    , m_disableWidget(new QWidget(this))
    , m_airplaneModeWidget(new QWidget(this))
    , m_airplaneModeLabel(new DTipLabel(QString(), m_airplaneModeWidget))
    , m_adaptersManager(adapterManager)
    , m_settingBtn(new JumpSettingButton(QIcon::fromTheme("bluetooth-open"), tr("Bluetooth settings"), this))
    , m_mainLayout(new QVBoxLayout(this))
    , m_contentLayout(new QVBoxLayout(m_contentWidget))
    , m_airPlaneModeInter(new DBusAirplaneMode("org.deepin.dde.AirplaneMode1", "/org/deepin/dde/AirplaneMode1", QDBusConnection::systemBus(), this))
    , m_airplaneModeEnable(false)
    , m_minHeight(0)
{
    initUi();
    initConnect();
}

bool BluetoothApplet::poweredInitState()
{
    foreach (const auto adapter, m_adapterItems) {
        if (adapter->adapter()->powered()) {
            return true;
        }
    }

    return false;
}

bool BluetoothApplet::hasAdapter()
{
    return m_adaptersManager->adaptersCount();
}

void BluetoothApplet::setAdapterRefresh()
{
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem->adapter()->discover())
            m_adaptersManager->adapterRefresh(adapterItem->adapter());
    }
    updateSize();
}

void BluetoothApplet::setAdapterPowered(bool state)
{
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem)
            m_adaptersManager->setAdapterPowered(adapterItem->adapter(), state);
    }
}

QStringList BluetoothApplet::connectedDevicesName()
{
    QStringList deviceList;
    for (BluetoothAdapterItem *adapterItem : m_adapterItems) {
        if (adapterItem)
            deviceList << adapterItem->connectedDevicesName();
    }

    return deviceList;
}

void BluetoothApplet::onAdapterAdded(Adapter *adapter)
{
    if (m_adapterItems.contains(adapter->id())) {
        onAdapterRemoved(m_adapterItems.value(adapter->id())->adapter());
    }

    BluetoothAdapterItem *adapterItem = new BluetoothAdapterItem(adapter, this);
    connect(adapterItem, &BluetoothAdapterItem::requestSetAdapterPower, this, &BluetoothApplet::onSetAdapterPower);
    connect(adapterItem, &BluetoothAdapterItem::connectDevice, m_adaptersManager, &AdaptersManager::connectDevice);
    connect(adapterItem, &BluetoothAdapterItem::deviceCountChanged, this, &BluetoothApplet::updateSize);
    connect(adapterItem, &BluetoothAdapterItem::adapterPowerChanged, this, &BluetoothApplet::updateBluetoothPowerState);
    connect(adapterItem, &BluetoothAdapterItem::deviceStateChanged, this, &BluetoothApplet::deviceStateChanged);
    connect(adapterItem, &BluetoothAdapterItem::requestRefreshAdapter, m_adaptersManager, &AdaptersManager::adapterRefresh);

    m_adapterItems.insert(adapter->id(), adapterItem);

    // 如果开启了飞行模式，置灰蓝牙适配器使能开关
    foreach (const auto item, m_adapterItems) {
        item->setStateBtnEnabled(!m_airPlaneModeInter->enabled());
    }

    m_contentLayout->insertWidget(m_contentLayout->count() - 1, adapterItem, 0, Qt::AlignTop);
    updateBluetoothPowerState();
    updateSize();

    if (m_adapterItems.size()) {
        emit justHasAdapter();
    }
}

void BluetoothApplet::onAdapterRemoved(Adapter *adapter)
{
    m_contentLayout->removeWidget(m_adapterItems.value(adapter->id()));
    m_adapterItems.value(adapter->id())->deleteLater();
    m_adapterItems.remove(adapter->id());
    if (m_adapterItems.isEmpty()) {
        emit noAdapter();
    }
    updateBluetoothPowerState();
    updateSize();
}

void BluetoothApplet::onSetAdapterPower(Adapter *adapter, bool state)
{
    m_adaptersManager->setAdapterPowered(adapter, state);
    updateSize();
}

void BluetoothApplet::updateBluetoothPowerState()
{
    const bool isOneDisable = m_adapterItems.size() == 1 && !m_adapterItems.first()->adapter()->powered();
    m_disableWidget->setVisible(m_airplaneModeEnable ? false : isOneDisable);
    m_airplaneModeWidget->setVisible(m_airplaneModeEnable);

    foreach (const auto item, m_adapterItems) {
        if (item->adapter()->powered()) {
            emit powerChanged(true);
            updateSize();
            return;
        }
    }
    emit powerChanged(false);
    updateSize();
}

void BluetoothApplet::initUi()
{
    setAccessibleName("BluetoothApplet");
    setContentsMargins(0, 0, 0, 0);

    m_contentLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(10, 0, 10, 0);
    m_contentLayout->addStretch();

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_contentWidget);
    m_contentWidget->setFixedWidth(Dock::DOCK_POPUP_WIDGET_WIDTH);

    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_scrollArea->setAutoFillBackground(true);
    m_scrollArea->viewport()->setAutoFillBackground(true);
    QPalette scrollAreaBackground = this->palette();
    scrollAreaBackground.setColor(QPalette::ColorRole::Window, Qt::transparent);
    m_scrollArea->setPalette(scrollAreaBackground);

    // QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    new TouchScrollFilter(m_scrollArea);

    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 10, 0, 0);
    m_mainLayout->addWidget(m_scrollArea);

    // 飞行模式跳转tip
    QVBoxLayout *airplaneLayout = new QVBoxLayout(m_airplaneModeWidget);
    airplaneLayout->setContentsMargins(20, 0, 10, 0);
    airplaneLayout->setSpacing(0);
    m_airplaneModeLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_airplaneModeLabel->setText(tr("Disable [Airplane Mode](#) first if you want to connect to a Bluetooth"));
    m_airplaneModeLabel->setTextFormat(Qt::MarkdownText);
    m_airplaneModeLabel->setWordWrap(true);
    DFontSizeManager::instance()->bind(m_airplaneModeLabel, DFontSizeManager::T8);
    airplaneLayout->addWidget(m_airplaneModeLabel, 0, Qt::AlignTop);
    airplaneLayout->addStretch(1);
    m_airplaneModeWidget->setVisible(false);
    m_airplaneModeWidget->setFixedWidth(ItemWidth);
    m_mainLayout->addWidget(m_airplaneModeWidget);

    QToolButton *disableIcon = new QToolButton(m_disableWidget);
    disableIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    disableIcon->setIcon(QIcon::fromTheme("bluetooth_disable"));
    disableIcon->setIconSize(QSize(96, 96));
    disableIcon->setFixedSize(96, 96);

    DLabel *disableText = new DLabel(tr("Turned off"), m_disableWidget);
    disableText->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(disableText, DFontSizeManager::T8);

    QVBoxLayout *lay = new QVBoxLayout(m_disableWidget);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 20);
    lay->addStretch();
    lay->addWidget(disableIcon, 0, Qt::AlignCenter);
    lay->addSpacing(2);
    lay->addWidget(disableText, 0, Qt::AlignCenter);
    lay->addStretch();
    m_disableWidget->setFixedWidth(ItemWidth);

    m_mainLayout->addWidget(m_disableWidget);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(10, 10, 10, 10);
    btnLayout->addWidget(m_settingBtn);
    m_settingBtn->setDccPage("bluetooth", QString());
    m_settingBtn->setFixedHeight(DeviceItemHeight);
    m_mainLayout->addLayout(btnLayout, Qt::AlignBottom | Qt::AlignVCenter);

    setAirplaneModeEnabled(m_airPlaneModeInter->enabled());
    updateBluetoothPowerState();
    updateSize();
}

void BluetoothApplet::initConnect()
{
    connect(m_adaptersManager, &AdaptersManager::adapterIncreased, this, &BluetoothApplet::onAdapterAdded);
    connect(m_adaptersManager, &AdaptersManager::adapterDecreased, this, &BluetoothApplet::onAdapterRemoved);

    connect(m_settingBtn, &JumpSettingButton::showPageRequestWasSended, this, &BluetoothApplet::requestHideApplet);

    connect(m_airPlaneModeInter, &DBusAirplaneMode::EnabledChanged, this, &BluetoothApplet::setAirplaneModeEnabled);
    connect(m_airPlaneModeInter, &DBusAirplaneMode::EnabledChanged, this, [this](bool enabled) {
        foreach (const auto item, m_adapterItems) {
            item->setStateBtnEnabled(!enabled);
        }
        m_disableWidget->setVisible(false);
        m_airplaneModeWidget->setVisible(enabled);
        updateSize();
    });

    connect(m_airplaneModeLabel, &DTipLabel::linkActivated, this, [this] {
        auto *activation = new tray::XdgActivation(this);
        connect(activation, &tray::XdgActivation::tokenReady, this, [activation](const QString &token) {
            QStringList args {"--by-user", "org.deepin.dde.control-center"};
            if (!token.isEmpty())
                args << "-e" << "XDG_ACTIVATION_TOKEN=" + token;
            args << "--" << "-p" << "network/airplaneMode";
            QProcess::startDetached("dde-am", args);
            activation->deleteLater();
        }, Qt::SingleShotConnection);
        activation->requestToken();
        Q_EMIT requestHideApplet();
    });
}

void BluetoothApplet::setAirplaneModeEnabled(bool enable)
{
    if (m_airplaneModeEnable == enable)
        return;

    m_airplaneModeEnable = enable;
}

void BluetoothApplet::updateSize()
{
    int height = 0;

    // 各蓝牙适配器高度之和
    foreach (const auto item, m_adapterItems) {
        height += item->sizeHint().height();
    }
    int adapterHeight = height;

    // 加上蓝牙设置选项的高度
    static const int settingHeight = DeviceItemHeight + 10 + 10;
    height += settingHeight;

    // 最大的高度为显示8个设备的高度
    static const int maxHeight = (TitleHeight + TitleSpace) + (MaxDeviceCount * DeviceItemHeight) + ((MaxDeviceCount-1) * 10) + settingHeight;

    // 如果比允许的最大高度还大，则以最大高度为准
    height = qMin(maxHeight, height);

    // 如果比快捷面板允许的最小高度还小，则以快捷面板允许的最小高度为准
    const int hMargins = m_mainLayout->contentsMargins().top() + m_mainLayout->contentsMargins().bottom();
    height = qMax(m_minHeight - hMargins, height);

    // 加上飞行模式提示控件高度
    m_airplaneModeWidget->isVisibleTo(this) ? m_airplaneModeWidget->setFixedHeight(height - settingHeight - adapterHeight) : m_airplaneModeWidget->setFixedHeight(0);

    // 加上提示控件高度
    m_disableWidget->isVisibleTo(this) ? m_disableWidget->setFixedHeight(height - settingHeight - adapterHeight) : m_disableWidget->setFixedHeight(0);

    // 设置滚动区高度
    const int scrollAreaHeight = height - settingHeight - m_disableWidget->height() - m_airplaneModeWidget->height();
    m_scrollArea->setFixedHeight(scrollAreaHeight);
    // 确保内容控件高度至少等于适配器高度，并且不会超过滚动区域的高度太多
    m_contentWidget->setMinimumHeight(qMax(adapterHeight, scrollAreaHeight));

    // top and bottom margin
    height += hMargins;

    setFixedSize(ItemWidth, height);
}

void BluetoothApplet::updateMinHeight(int minHeight)
{
    m_minHeight = minHeight;
    updateSize();
}
