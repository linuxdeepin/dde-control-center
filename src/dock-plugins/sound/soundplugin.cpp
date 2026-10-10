// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "soundplugin.h"
#include "utils.h"
#include "plugins-logging-category.h"

Q_LOGGING_CATEGORY(DOCK_SOUND, "org.deepin.dde.dock.sound")

SoundPlugin::SoundPlugin(QObject *parent)
    : QObject(parent),
      m_soundItem(nullptr)
    , m_soundWidget(nullptr)
{
}

const QString SoundPlugin::pluginName() const
{
    return "sound";
}

const QString SoundPlugin::pluginDisplayName() const
{
    return tr("Sound");
}

void SoundPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    if (m_soundItem)
        return;

    m_soundWidget.reset(new SoundQuickPanel);
    m_soundWidget->setFixedHeight(60);
    m_soundItem.reset(new SoundView);

    m_proxyInter->itemAdded(this, SOUND_KEY);
    connect(m_soundWidget.data(), &SoundQuickPanel::rightIconClick, this, [ this, proxyInter ] {
        proxyInter->requestSetAppletVisible(this, SOUND_KEY, true);
    });
    connect(m_soundItem.data(), &SoundView::requestHideApplet, this, [this] {
        m_proxyInter->requestSetAppletVisible(this, SOUND_KEY, false);
    });
}

QWidget *SoundPlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == Dock::QUICK_ITEM_KEY) {
        return m_soundWidget.data();
    }

    if (itemKey == SOUND_KEY) {
        return m_soundItem->dockIcon();
    }

    return nullptr;
}

QWidget *SoundPlugin::itemTipsWidget(const QString &itemKey)
{
    if (itemKey == SOUND_KEY) {
        return m_soundItem->tipsWidget();
    }

    return nullptr;
}

QWidget *SoundPlugin::itemPopupApplet(const QString &itemKey)
{
    if (itemKey == SOUND_KEY) {
        return m_soundItem->popupApplet();
    }

    return nullptr;
}

const QString SoundPlugin::itemContextMenu(const QString &itemKey)
{
    if (itemKey == SOUND_KEY) {
        return m_soundItem->contextMenu();
    }

    return QString();
}

void SoundPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    if (itemKey == SOUND_KEY) {
        m_soundItem->invokeMenuItem(menuId, checked);
    }
}

int SoundPlugin::itemSortKey(const QString &itemKey)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    return m_proxyInter->getValue(this, key, -1).toInt();
}

void SoundPlugin::setSortKey(const QString &itemKey, const int order)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);

    m_proxyInter->saveValue(this, key, order);
}

void SoundPlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == SOUND_KEY) {
        m_soundItem->refreshIcon();
    }
}

void SoundPlugin::pluginSettingsChanged()
{
    refreshPluginItemsVisible();
}

void SoundPlugin::refreshPluginItemsVisible()
{
    m_proxyInter->itemAdded(this, SOUND_KEY);
}

QString SoundPlugin::message(const QString &message)
{
    QJsonObject msgObj = Utils::getRootObj(message);
    if (msgObj.isEmpty()) {
        return "{}";
    }

    QJsonObject retObj;
    QString cmdType = msgObj.value(Dock::MSG_TYPE).toString();
    if (cmdType == Dock::MSG_SET_APPLET_MIN_HEIGHT) {
        const int minHeight = msgObj.value(Dock::MSG_DATA).toInt(-1);
        if (m_soundItem && minHeight > 0)
            m_soundItem->setAppletMinHeight(minHeight);
    }

    return Utils::toJson(retObj);
}
