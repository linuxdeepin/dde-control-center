// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wirelesscastingplugin.h"

#include "wirelesscastingapplet.h"
#include "wirelesscastingitem.h"

#include <QLabel>

namespace dde {
namespace wirelesscasting {

WirelessCastingPlugin::WirelessCastingPlugin(QObject *parent)
    : QObject(parent)
    , m_wirelessCastingItem(nullptr)
    , m_messageCallback(nullptr)
{
    QTranslator *translator = new QTranslator(this);
    if (translator->load(QString("/usr/share/dock-wirelesscasting-plugin/translations/dock-wirelesscasting-plugin_%1.qm").arg(QLocale::system().name()))) {
        QCoreApplication::installTranslator(translator);
    }
}

void WirelessCastingPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    if (m_wirelessCastingItem)
        return;
    m_wirelessCastingItem = new WirelessCastingItem();
    if (m_wirelessCastingItem->canCasting())
        m_proxyInter->itemAdded(this, WIRELESS_CASTING_KEY);

    connect(m_wirelessCastingItem, &WirelessCastingItem::canCastingChanged, this, [this](bool canCasting) {
        if (canCasting)
            m_proxyInter->itemAdded(this, WIRELESS_CASTING_KEY);
        else
            m_proxyInter->itemRemoved(this, WIRELESS_CASTING_KEY);

        notifySupportFlagChanged(canCasting);
    });
    connect(m_wirelessCastingItem, &WirelessCastingItem::requestExpand, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, WIRELESS_CASTING_KEY, true);
    });
    connect(m_wirelessCastingItem, &WirelessCastingItem::requestHideApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, WIRELESS_CASTING_KEY, false);
    });
}

void WirelessCastingPlugin::pluginStateSwitched() { }

bool WirelessCastingPlugin::pluginIsDisable()
{
    return false;
}

QWidget *WirelessCastingPlugin::itemWidget(const QString &itemKey)
{
    if ("quick_item_key" == itemKey)
        return m_wirelessCastingItem->quickPanelWidget();
    if (WIRELESS_CASTING_KEY == itemKey)
        return m_wirelessCastingItem->trayIcon();
    return nullptr;
}

QWidget *WirelessCastingPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey)
    return m_wirelessCastingItem->tips();
}

QWidget *WirelessCastingPlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey)
    return m_wirelessCastingItem->appletWidget();
}

const QString WirelessCastingPlugin::itemContextMenu(const QString &itemKey)
{
    if (itemKey == WIRELESS_CASTING_KEY) {
        return m_wirelessCastingItem->contextMenu();
    }
    return QString();
}

void WirelessCastingPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    if (itemKey == WIRELESS_CASTING_KEY) {
        m_wirelessCastingItem->invokeMenuItem(menuId, checked);
    }
}

int WirelessCastingPlugin::itemSortKey(const QString &itemKey)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    return m_proxyInter->getValue(this, key, 4).toInt();
}

void WirelessCastingPlugin::setSortKey(const QString &itemKey, const int order)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    m_proxyInter->saveValue(this, key, order);
}

void WirelessCastingPlugin::refreshIcon(const QString &itemKey) { Q_UNUSED(itemKey) }

void WirelessCastingPlugin::pluginSettingsChanged() { }

static QString toJson(const QJsonObject &jsonObj)
{
    QJsonDocument doc;
    doc.setObject(jsonObj);
    return doc.toJson();
}

QString WirelessCastingPlugin::message(const QString &msg)
{
    QJsonParseError jsonParseError;
    const QJsonDocument &resultDoc = QJsonDocument::fromJson(msg.toLocal8Bit(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError || resultDoc.isEmpty()) {
        qWarning() << "Result json parse error";
        return "{}";
    }

    QJsonObject retObj;
    const auto &msgObj = resultDoc.object();
    const QString &cmdType = msgObj.value(Dock::MSG_TYPE).toString();
    if (cmdType == Dock::MSG_SET_APPLET_MIN_HEIGHT) {
        const int minHeight = msgObj.value(Dock::MSG_DATA).toInt(-1);
        if (m_wirelessCastingItem && minHeight > 0)
            m_wirelessCastingItem->setAppletMinHeight(minHeight);
    }else if (cmdType == Dock::MSG_GET_SUPPORT_FLAG) {
        if (m_wirelessCastingItem)
            retObj[Dock::MSG_SUPPORT_FLAG] = m_wirelessCastingItem->canCasting();
    } else if (cmdType == Dock::MSG_APPLET_CONTAINER) {
        if (m_wirelessCastingItem && m_wirelessCastingItem->appletWidget()) {
            auto *applet = dynamic_cast<dde::wirelesscasting::WirelessCastingApplet *>(m_wirelessCastingItem->appletWidget());
            if (applet)
                applet->onContainerChanged(msgObj.value(Dock::MSG_DATA).toInt(Dock::APPLET_CONTAINER_DOCK));
        }
    }

    return toJson(retObj);
}

const QString WirelessCastingPlugin::pluginDisplayName() const
{
    return tr("Multiple Displays");
}

void WirelessCastingPlugin::notifySupportFlagChanged(bool supportFlag)
{
    if (!m_messageCallback) {
        return;
    }

    QJsonObject msg;
    msg[Dock::MSG_TYPE] = Dock::MSG_SUPPORT_FLAG_CHANGED;
    msg[Dock::MSG_DATA] = supportFlag;
    m_messageCallback(this, toJson(msg));
}


} // namespace wirelesscasting
} // namespace dde
