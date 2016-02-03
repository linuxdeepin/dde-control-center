/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "diskmountplugin.h"


DiskMountPlugin::DiskMountPlugin()
{
    m_diskMount = new DBusDiskMount(this);
    connect(m_diskMount,&DBusDiskMount::DiskListChanged,this,&DiskMountPlugin::mountableDeviceChanged);

    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(onInitTimerTriggered()));
    initTimer->start(1000);

    m_item = new MainItem();
    connect(this, &DiskMountPlugin::dockModeChanged, m_item, &MainItem::onDockModeChanged);
}


void DiskMountPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
}

QString DiskMountPlugin::getPluginName()
{
    return "DiskMount plugin";
}

QStringList DiskMountPlugin::ids()
{
    return QStringList(m_id);
}

QString DiskMountPlugin::getName(QString)
{
    return getPluginName();
}

QString DiskMountPlugin::getTitle(QString)
{
    return getPluginName();
}

QString DiskMountPlugin::getCommand(QString)
{
    return "";
}

bool DiskMountPlugin::configurable(const QString &)
{
    return false;
}

bool DiskMountPlugin::enabled(const QString &)
{
    return true;
}

void DiskMountPlugin::setEnabled(const QString &, bool)
{

}

QWidget * DiskMountPlugin::getItem(QString)
{
    m_item->onDockModeChanged(m_mode);
    return m_item;
}

QWidget * DiskMountPlugin::getApplet(QString)
{
    return m_diskContent;
}

void DiskMountPlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
}

QString DiskMountPlugin::getMenuContent(QString)
{
    return "";
}

void DiskMountPlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    qWarning() << "Menu check:" << itemId << checked;
}

void DiskMountPlugin::onInitTimerTriggered()
{
    if (m_diskMount->isValid()) {
        qDebug() << "DiskMountPlugin: DBus data is ready!";
        sender()->deleteLater();

        m_diskContent = new DiskContent(m_id, m_proxy);

        mountableDeviceChanged();
        setMode(m_proxy->dockMode());
    }
}

void DiskMountPlugin::mountableDeviceChanged()
{
    bool hasMount = false;
    DiskInfoList tmpList = m_diskMount->diskList();
    for (DiskInfo info : tmpList) {
        if (info.canUnmount)
        {
            hasMount = true;
            break;
        }
    }

    if (hasMount == m_hasMount)
        return;
    else
        m_hasMount = hasMount;

    if (hasMount)
        m_proxy->itemAddedEvent(m_id);
    else{
        m_item->setParent(NULL);
        m_proxy->itemRemovedEvent(m_id);
    }
}

// private methods
void DiskMountPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    emit dockModeChanged(mode);

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeItemSize, m_id);
}

QJsonObject DiskMountPlugin::createMenuItem(QString itemId, QString itemName, bool checkable, bool checked)
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


DiskMountPlugin::~DiskMountPlugin()
{
    if (m_diskContent)
        m_diskContent->deleteLater();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dde-dock-diskmount-plugin, DiskMountPlugin)
#endif // QT_VERSION < 0x050000
