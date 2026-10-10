// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "eyecomfortmodeplugin.h"
#include "eyecomfortmodecontroller.h"
#include "utils.h"
#include "plugins-logging-category.h"

#define EYE_COMFORT_MODE_KEY "eye-comfort-mode-key"
DCORE_USE_NAMESPACE

Q_LOGGING_CATEGORY(EYE_COMFORT, "org.deepin.dde.dock.eye-comfort-mode")

EyeComfortModePlugin::EyeComfortModePlugin(QObject *parent)
    : QObject(parent)
    , m_eyeComfortModeItem(nullptr)
{
}

EyeComfortModePlugin::~EyeComfortModePlugin()
{
}

const QString EyeComfortModePlugin::pluginName() const
{
    return "eye-comfort-mode";
}

const QString EyeComfortModePlugin::pluginDisplayName() const
{
    return m_eyeComfortModeItem->displayName();
}

void EyeComfortModePlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    m_eyeComfortModeItem.reset(new EyeComfortModeItem);

    connect(&EyeComfortModeController::ref(), &EyeComfortModeController::supportColorTemperatureChanged, this, [this] (bool support) {
        support ? m_proxyInter->itemAdded(this, EYE_COMFORT_MODE_KEY) : m_proxyInter->itemRemoved(this, EYE_COMFORT_MODE_KEY);
        notifySupportFlagChanged(support);
    });
    if (!pluginIsDisable()) {
        m_proxyInter->itemAdded(this, EYE_COMFORT_MODE_KEY);
    }
    connect(m_eyeComfortModeItem.data(), &EyeComfortModeItem::requestHideApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, EYE_COMFORT_MODE_KEY, false);
    });
    connect(m_eyeComfortModeItem.data(), &EyeComfortModeItem::requestExpand, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, EYE_COMFORT_MODE_KEY, true);
    });
}

QWidget *EyeComfortModePlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        return m_eyeComfortModeItem.data();
    }

    if (itemKey == Dock::QUICK_ITEM_KEY) {
        return m_eyeComfortModeItem->quickPanel();
    }

    return nullptr;
}

QWidget *EyeComfortModePlugin::itemTipsWidget(const QString &itemKey)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        return m_eyeComfortModeItem->tipsWidget();
    }

    return nullptr;
}

QWidget *EyeComfortModePlugin::itemPopupApplet(const QString &itemKey)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        return m_eyeComfortModeItem->popupApplet();
    }

    return nullptr;
}

const QString EyeComfortModePlugin::itemContextMenu(const QString &itemKey)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        return m_eyeComfortModeItem->contextMenu();
    }

    return QString();
}

void EyeComfortModePlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        m_eyeComfortModeItem->invokeMenuItem(menuId, checked);
    }
}

int EyeComfortModePlugin::itemSortKey(const QString &itemKey)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    return m_proxyInter->getValue(this, key, -1).toInt();
}

void EyeComfortModePlugin::setSortKey(const QString &itemKey, const int order)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    m_proxyInter->saveValue(this, key, order);
}

void EyeComfortModePlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == EYE_COMFORT_MODE_KEY) {
        m_eyeComfortModeItem->refreshIcon();
    }
}

QString EyeComfortModePlugin::message(const QString &message)
{
    QJsonObject msgObj = Utils::getRootObj(message);
    if (msgObj.isEmpty()) {
        return "{}";
    }

    QJsonObject retObj;
    QString cmdType = msgObj.value(Dock::MSG_TYPE).toString();
    if (cmdType == Dock::MSG_SET_APPLET_MIN_HEIGHT) {
        const int height = msgObj.value(Dock::MSG_DATA).toInt(-1);
        if (m_eyeComfortModeItem && height > 0)
            m_eyeComfortModeItem->setAppletHeight(height);
    }

    return QString();
}

void EyeComfortModePlugin::notifySupportFlagChanged(bool supportFlag)
{
    if (!m_messageCallback) {
        return;
    }

    QJsonObject msg;
    msg[Dock::MSG_TYPE] = Dock::MSG_SUPPORT_FLAG_CHANGED;
    msg[Dock::MSG_DATA] = supportFlag;
    m_messageCallback(this, Utils::toJson(msg));
}
