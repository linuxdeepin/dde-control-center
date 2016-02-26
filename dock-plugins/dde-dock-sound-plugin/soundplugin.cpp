/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QFile>
#include <QTimer>

#include "soundplugin.h"

SoundPlugin::SoundPlugin()
{
    m_audio = new DBusAudio(this);
    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(onInitTimerTriggered()));
    initTimer->start(1000);
    initSettings();
}

void SoundPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    setMode(proxy->dockMode());
}

QString SoundPlugin::getPluginName()
{
    return tr("Sound");
}

QStringList SoundPlugin::ids()
{
    return QStringList(m_id);
}

QString SoundPlugin::getName(QString)
{
    return getPluginName();
}

QString SoundPlugin::getTitle(QString)
{
    return getPluginName();
}

QString SoundPlugin::getCommand(QString)
{
    return "dde-control-center sound";
}

QPixmap SoundPlugin::getIcon(QString)
{
    return SoundIcon::getDefaultSinkDockIcon(Dock::EfficientMode, 100);
}

bool SoundPlugin::configurable(const QString &)
{
    return true;
}

bool SoundPlugin::enabled(const QString &)
{
    return m_settings->value(settingEnabledKey(), true).toBool();    //default enabled
}

void SoundPlugin::setEnabled(const QString &id, bool enabled)
{
    if (id != m_id)
        return;

    m_settings->setValue(settingEnabledKey(), enabled);

    onEnabledChanged();
}

QWidget * SoundPlugin::getItem(QString)
{
    return (!enabled(m_id) || !m_audio->isValid()) ? NULL : m_item;
}

QWidget * SoundPlugin::getApplet(QString)
{
    if (m_audio->isValid())
        return m_soundContent;
    else
        return NULL;
}

void SoundPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (newMode != oldMode){
        setMode(newMode);
        onEnabledChanged();
    }
}

QString SoundPlugin::getMenuContent(QString)
{
    return "";
}

void SoundPlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    qWarning() << "Menu check:" << itemId << checked;
}

void SoundPlugin::onInitTimerTriggered()
{
    QTimer *t = qobject_cast<QTimer *>(sender());

    if (t && m_audio->isValid()) {
        qWarning() << "SoundPlugin: DBus data is ready!";
        t->stop();
        t->deleteLater();

        //init, add setting line to dock plugins setting frame
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, m_id);
        m_item = new MainItem();
        m_soundContent = new SoundContent(m_id, m_proxy);

        setMode(m_proxy->dockMode());
        onEnabledChanged();
    }
}

// private methods
void SoundPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-sound-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        m_settings->setValue(settingEnabledKey(), true);
    }
}

void SoundPlugin::onEnabledChanged()
{
    if (!m_item)
        return;

    m_proxy->itemRemovedEvent(m_id);
    m_item->setParent(NULL);

    if (enabled(m_id)) {
        m_proxy->itemAddedEvent(m_id);
    }
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, m_id);
}

void SoundPlugin::setMode(Dock::DockMode mode)
{
    if (!m_item)
        return;

    m_mode = mode;

    switch (mode) {
    case Dock::FashionMode:
        m_item->resize(Dock::APPLET_FASHION_ITEM_WIDTH,Dock::APPLET_FASHION_ITEM_HEIGHT);
        break;
    case Dock::EfficientMode:
        m_item->resize(Dock::APPLET_EFFICIENT_ITEM_WIDTH,Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
        break;
    case Dock::ClassicMode:
        m_item->resize(Dock::APPLET_CLASSIC_ITEM_WIDTH,Dock::APPLET_CLASSIC_ITEM_HEIGHT);
        break;
    default:
        break;
    }

    m_item->setDockMode(mode);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeItemSize, m_id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, m_id);
}

QString SoundPlugin::settingEnabledKey()
{
    return QString("%1/enabled").arg(m_mode);
}

QJsonObject SoundPlugin::createMenuItem(QString itemId, QString itemName, bool checkable, bool checked)
{
    QJsonObject itemObj;

    itemObj.insert("itemId", itemId);
    itemObj.insert("itemText", itemName);
    itemObj.insert("itemIcon", "");
    itemObj.insert("itemIconHover", "");
    itemObj.insert("itemIconInactive", "");
    itemObj.insert("itemExtra", "");
    itemObj.insert("isActive", true);
    itemObj.insert("isCheckable", checkable);
    itemObj.insert("checked", checked);
    itemObj.insert("itemSubMenu", QJsonObject());

    return itemObj;
}

SoundPlugin::~SoundPlugin()
{
    if (m_soundContent)
        m_soundContent->deleteLater();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-sound-plugin, SoundPlugin)
#endif // QT_VERSION < 0x050000
