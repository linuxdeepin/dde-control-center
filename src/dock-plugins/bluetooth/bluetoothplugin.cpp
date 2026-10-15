// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothplugin.h"
#include "utils.h"
#include "componments/bluetoothapplet.h"
#include "plugins-logging-category.h"

#define STATE_KEY  "enable"

Q_LOGGING_CATEGORY(BLUETOOTH, "org.deepin.dde.dock.bluetooth")

BluetoothPlugin::BluetoothPlugin(QObject *parent)
    : QObject(parent)
    , m_adapterManager(nullptr)
    , m_bluetoothItem(nullptr)
    , m_messageCallback(nullptr)
    , m_enableState(true)
{
}

const QString BluetoothPlugin::pluginName() const
{
    return "bluetooth";
}

const QString BluetoothPlugin::pluginDisplayName() const
{
    return tr("Bluetooth");
}

void BluetoothPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    if (m_bluetoothItem)
        return;

    m_adapterManager = new AdaptersManager(this);
    m_bluetoothItem.reset(new BluetoothItem(m_adapterManager));

    connect(m_bluetoothItem.data(), &BluetoothItem::justHasAdapter, this, [&] {
        setEnableState(true);
        refreshPluginItemsVisible();
    });
    connect(m_bluetoothItem.data(), &BluetoothItem::noAdapter, this, [&] {
        setEnableState(false);
        refreshPluginItemsVisible();
    });
    connect(m_bluetoothItem.data(), &BluetoothItem::requestExpand, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, BLUETOOTH_KEY, true);
    });
    connect(m_bluetoothItem.data(), &BluetoothItem::requestHideApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, BLUETOOTH_KEY, false);
    });
    setEnableState(m_bluetoothItem->hasAdapter());
    refreshPluginItemsVisible();
}

void BluetoothPlugin::pluginStateSwitched()
{
    if(!m_proxyInter)
        return;

    m_proxyInter->saveValue(this, STATE_KEY, pluginIsDisable());

    refreshPluginItemsVisible();
}

bool BluetoothPlugin::pluginIsDisable()
{
    if(!m_proxyInter)
        return false;

    return !m_proxyInter->getValue(this, STATE_KEY, m_enableState).toBool();
}

QWidget *BluetoothPlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == BLUETOOTH_KEY) {
        return m_bluetoothItem->dockItemWidget();
    }

    if (itemKey == Dock::QUICK_ITEM_KEY) {
        return m_bluetoothItem->quickPanel();
    }

    return nullptr;
}

QWidget *BluetoothPlugin::itemTipsWidget(const QString &itemKey)
{
    if (itemKey == BLUETOOTH_KEY) {
        return m_bluetoothItem->tipsWidget();
    }

    return nullptr;
}

QWidget *BluetoothPlugin::itemPopupApplet(const QString &itemKey)
{
    if (itemKey == BLUETOOTH_KEY) {
        return m_bluetoothItem->popupApplet();
    }

    return nullptr;
}

const QString BluetoothPlugin::itemContextMenu(const QString &itemKey)
{
    if (itemKey == BLUETOOTH_KEY) {
        return m_bluetoothItem->contextMenu();
    }

    return QString();
}

void BluetoothPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    if (itemKey == BLUETOOTH_KEY) {
        m_bluetoothItem->invokeMenuItem(menuId, checked);
    }
}

int BluetoothPlugin::itemSortKey(const QString &itemKey)
{
    if(!m_proxyInter)
        return -1;

    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    return m_proxyInter->getValue(this, key, -1).toInt();
}

void BluetoothPlugin::setSortKey(const QString &itemKey, const int order)
{
    if(!m_proxyInter)
        return;

    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    m_proxyInter->saveValue(this, key, order);
}

void BluetoothPlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == BLUETOOTH_KEY) {
        m_bluetoothItem->refreshIcon();
    }
}

void BluetoothPlugin::pluginSettingsChanged()
{
    refreshPluginItemsVisible();
}

void BluetoothPlugin::refreshPluginItemsVisible()
{
    if(!m_proxyInter)
        return;

    if (pluginIsDisable() || !m_enableState)
        m_proxyInter->itemRemoved(this, BLUETOOTH_KEY);
    else
        m_proxyInter->itemAdded(this, BLUETOOTH_KEY);
}

QString BluetoothPlugin::message(const QString &message)
{
    QJsonObject msgObj = Utils::getRootObj(message);
    if (msgObj.isEmpty()) {
        return "{}";
    }

    QJsonObject retObj;
    QString cmdType = msgObj.value(Dock::MSG_TYPE).toString();
    if (cmdType == Dock::MSG_GET_SUPPORT_FLAG) {
        retObj[Dock::MSG_SUPPORT_FLAG] = m_enableState;
    } else if (cmdType == Dock::MSG_SET_APPLET_MIN_HEIGHT) {
        const int minHeight = msgObj.value(Dock::MSG_DATA).toInt(-1);
        if (m_bluetoothItem && m_bluetoothItem->popupApplet()) {
            m_bluetoothItem->popupApplet()->updateMinHeight(minHeight);
        }
    }

    return Utils::toJson(retObj);
}

void BluetoothPlugin::notifySupportFlagChanged(bool supportFlag)
{
    if (!m_messageCallback) {
        return;
    }

    QJsonObject msg;
    msg[Dock::MSG_TYPE] = Dock::MSG_SUPPORT_FLAG_CHANGED;
    msg[Dock::MSG_DATA] = supportFlag;
    m_messageCallback(this, Utils::toJson(msg));
}

void BluetoothPlugin::setEnableState(bool enable)
{
    if (m_enableState != enable) {
        m_enableState = enable;
        notifySupportFlagChanged(m_enableState);
    }
}

