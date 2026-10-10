// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnessplugin.h"
#include "brightness-constants.h"

#include "utils.h"

DCORE_USE_NAMESPACE

Q_LOGGING_CATEGORY(BRIGHTNESS, "org.deepin.dde.dock.brightness")

BrightnessPlugin::BrightnessPlugin(QObject *parent)
    : QObject(parent)
    , m_quickPanelWidget(nullptr)
    , m_item(nullptr)
{
}

BrightnessPlugin::~BrightnessPlugin()
{
    if (m_quickPanelWidget) {
        delete m_quickPanelWidget;
        m_quickPanelWidget = nullptr;
    }
}

const QString BrightnessPlugin::pluginName() const
{
    return "dde-brightness";
}

const QString BrightnessPlugin::pluginDisplayName() const
{
    return tr("Brightness");
}

void BrightnessPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (m_quickPanelWidget)
        return;

    BrightnessController::ref().init();
    m_quickPanelWidget = new BrightnessQuickPanel;
    m_item = new BrightnessItem;

    connect(&BrightnessController::ref(), &BrightnessController::supportBrightnessChanged, this, [this] (bool support) {
        support ? m_proxyInter->itemAdded(this, pluginName()) : m_proxyInter->itemRemoved(this, pluginName());
        notifySupportFlagChanged(support);
    });

    if (!pluginIsDisable() && BrightnessController::ref().supportBrightness()) {
        m_proxyInter->itemAdded(this, pluginName());
    }

    connect(m_item, &BrightnessItem::requestHideApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, pluginName(), false);
    });
    connect(m_quickPanelWidget, &BrightnessQuickPanel::requestShowApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, pluginName(), true);
    });
}

QWidget *BrightnessPlugin::itemWidget(const QString &itemKey)
{
    if (!m_item)
        return nullptr;

    if (itemKey == pluginName()) {
        return m_item->itemWidget();
    }

    if (itemKey == Dock::QUICK_ITEM_KEY) {
        return m_quickPanelWidget;
    }

    return nullptr;
}

QWidget *BrightnessPlugin::itemTipsWidget(const QString &itemKey)
{
    if (!m_item)
        return nullptr;

    if (itemKey == pluginName()) {
        return m_item->tipsWidget();
    }

    return nullptr;
}

QWidget *BrightnessPlugin::itemPopupApplet(const QString &itemKey)
{
    if (!m_item)
        return nullptr;

    if (itemKey == pluginName()) {
        return m_item->popupApplet();
    }

    return nullptr;
}

const QString BrightnessPlugin::itemContextMenu(const QString &itemKey)
{
    if (!m_item)
        return nullptr;

    if (itemKey == pluginName()) {
        return m_item->contextMenu();
    }

    return QString();
}

void BrightnessPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    if (m_item && itemKey == pluginName()) {
        m_item->invokeMenuItem(menuId, checked);
    }
}

QString BrightnessPlugin::message(const QString &message)
{
    QJsonObject msgObj = Utils::getRootObj(message);
    if (msgObj.isEmpty()) {
        return "{}";
    }

    QJsonObject retObj;
    QString cmdType = msgObj.value(Dock::MSG_TYPE).toString();
    if (cmdType == Dock::MSG_GET_SUPPORT_FLAG) {
        retObj[Dock::MSG_SUPPORT_FLAG] = BrightnessController::ref().supportBrightness();
    } else if (cmdType == Dock::MSG_SET_APPLET_MIN_HEIGHT) {
        const int minHeight = msgObj.value(Dock::MSG_DATA).toInt(-1);
        if (m_item) {
            m_item->popupApplet()->setAppletMinHeight(minHeight);
        }
    } else if (cmdType == Dock::MSG_APPLET_CONTAINER) {
        if (m_item) {
            m_item->popupApplet()->onContainerChanged(msgObj.value(Dock::MSG_DATA).toInt(Dock::APPLET_CONTAINER_DOCK));
        }
    }

    return Utils::toJson(retObj);
}

void BrightnessPlugin::notifySupportFlagChanged(bool supportFlag)
{
    if (!m_messageCallback) {
        return;
    }

    QJsonObject msg;
    msg[Dock::MSG_TYPE] = Dock::MSG_SUPPORT_FLAG_CHANGED;
    msg[Dock::MSG_DATA] = supportFlag;
    m_messageCallback(this, Utils::toJson(msg));
}

