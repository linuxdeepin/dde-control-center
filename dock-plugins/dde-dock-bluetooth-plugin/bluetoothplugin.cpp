/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QSvgWidget>

#include <dde-dock/dockpluginproxyinterface.h>

#include "bluetoothplugin.h"
#include "bluetoothobject.h"
#include "adapterwidget.h"

BluetoothPlugin::BluetoothPlugin(QObject *parent) :
    QObject(parent)
{
    m_settings = new QSettings("deepin", "dde-dock-bluetooth-plugin", this);

    qDebug() << "Bluetooth: BluetoothPlugin created";
}

BluetoothPlugin::~BluetoothPlugin()
{
    qDeleteAll(m_mapIdToButton.values());
}

QString BluetoothPlugin::getPluginName()
{
    return "Bluetooth plugin";
}

void BluetoothPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    qDebug() << "Bluetooth: BluetoothPlugin init";

    if(!m_bluetooth) {
        m_bluetooth = new BluetoothObject(this);

        connect(m_bluetooth, &BluetoothObject::adapterAdd, this, [this](const QString &path) {
            addItem(path);

            BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(path);

            if(info) {
                connect(info->widget, &AdapterWidget::sizeChanged, this, [this, path] {
                    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, path);
                });
            }

            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, path);
        });

        connect(m_bluetooth, &BluetoothObject::adapterRemove, this, [this](const QString &path) {
            removeItem(path);
            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, path);
        });

        connect(m_bluetooth, &BluetoothObject::adapterPropertyChanged, this, [this](const QString &path) {
            QSvgWidget *widget = m_mapIdToButton.value(path, nullptr);

            if(widget) {
                BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(path);

                const QString icon_path = QString(":/dark/images/bluetooth_%1.svg").arg(info && info->powered ? "on" : "off");

                widget->load(icon_path);
            }
        });
    }
}

void BluetoothPlugin::removeItem(const QString &id)
{
    m_proxy->itemRemovedEvent(id);

    QSvgWidget *label = m_mapIdToButton.value(id, nullptr);
    if(label)
        label->deleteLater();

    m_mapIdToButton.remove(id);
}

void BluetoothPlugin::addItem(const QString &path)
{
    // already exists.
    if (m_mapIdToButton.contains(path))
        return;

    QSvgWidget *widget = new QSvgWidget;
    m_mapIdToButton[path] = widget;

    m_proxy->itemAddedEvent(path);
}

void BluetoothPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    for (const QString& id : ids()) {
//        if (newMode == Dock::FashionMode) {
//            removeItem(id);
//        } else if(oldMode == Dock::FashionMode){
//            addItem(id);
//        }
        if (oldMode != Dock::FashionMode)
            removeItem(id);
        if (newMode != Dock::FashionMode)
            addItem(id);

        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    }
}

QStringList BluetoothPlugin::ids()
{
    return m_bluetooth->getIds();
}

QString BluetoothPlugin::getName(QString id)
{
    BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(id);

    return info ? info->name : "";
}

QString BluetoothPlugin::getTitle(QString id)
{
    return getName(id);
}

QString BluetoothPlugin::getCommand(QString)
{
    return "dde-control-center bluetooth";
}

QPixmap BluetoothPlugin::getIcon(QString id)
{
    Q_UNUSED(id)

    QPixmap icon(":/dark/images/bluetooth_on.svg");

    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool BluetoothPlugin::configurable(const QString &id)
{
    Q_UNUSED(id)

    return m_proxy->dockMode() != Dock::FashionMode ? m_mapIdToButton.contains(id) : false;
}

QString BluetoothPlugin::settingEnabledKey(const QString &id)
{
    return QString("%1_%2_enabled").arg(id).arg(m_proxy->dockMode());
}

bool BluetoothPlugin::enabled(const QString &id)
{
    return  m_proxy->dockMode() != Dock::FashionMode
            ? m_settings->value(settingEnabledKey(id), true).toBool()
            : false;    //default enabled
}

void BluetoothPlugin::setEnabled(const QString &id, bool enabled)
{
    m_settings->setValue(settingEnabledKey(id), enabled);

    if(enabled) {
        addItem(id);
    } else {
        removeItem(id);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
}

QWidget *BluetoothPlugin::getItem(QString id)
{
    if (!enabled(id))
        return nullptr;

    QSvgWidget *item = m_mapIdToButton.value(id, nullptr);
    if (!item)
        return nullptr;

    BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(id);
    const QString icon_path = QString(":/dark/images/bluetooth_%1.svg").arg(info && info->powered ? "on" : "off");
    item->load(icon_path);

    if (m_proxy->dockMode() == Dock::EfficientMode)
        item->resize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
    else
        item->resize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);

    item->setVisible(true);

    return item;
}

QWidget *BluetoothPlugin::getApplet(QString id)
{
    BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(id);

    if(info && info->widget)
        info->widget->setFixedHeight(info->widget->getHeightHint());

    return info ? info->widget : NULL;
}

QString BluetoothPlugin::getMenuContent(QString id)
{
    return id;
}

void BluetoothPlugin::invokeMenuItem(QString, QString, bool)
{
}

