/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "modemnetworklistitem.h"
#include "networkglobal.h"
#include "dbus/dbusnetwork.h"
#include "networkgenericlistitem.h"
#include "networkmainwidget.h"

ModemNetworkListItem::ModemNetworkListItem(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("Modem Network"), dbus, parent)
{
    init();
}

void ModemNetworkListItem::onConnectsChanged()
{
    const QJsonDocument &json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Mobile].toArray()) {
        const QJsonObject &json_object = value.toObject();

        if(json_object["Uuid"].toString() == uniqueUuid()) {
            m_item->updateInfoByMap(json_object.toVariantMap());
            m_item->setConnectPath(m_item->path());
            m_item->setTitle(json_object["Id"].toString());
            break;
        }
    }
}

void ModemNetworkListItem::init()
{
    m_item = new NetworkGenericListItem(m_dbusNetwork);
    m_item->setTitle(tr("Restore the connection"));
    m_item->setLoading(true);

    listWidget()->addWidget(m_item);

    connect(m_item, &NetworkGenericListItem::clicked, this, [this] {
        if(m_item->loading())
            return;

        if(m_item->checked())
            m_item->onArrowClicked();
        else
            m_dbusNetwork->ActivateConnection(uniqueUuid(), QDBusObjectPath(path()));
    });

    connect(this, &ModemNetworkListItem::uniqueUuidChanged, this, &ModemNetworkListItem::onConnectsChanged);
    connect(this, &ModemNetworkListItem::uniqueUuidChanged, m_item, &NetworkGenericListItem::setUuid);
    connect(this, &ModemNetworkListItem::pathChanged, m_item, &NetworkGenericListItem::setDevicePath);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &ModemNetworkListItem::onConnectsChanged);
    connect(this, &ModemNetworkListItem::stateChanged, this, [this](int state) {
        if(state >= DeviceState::Prepare && state <= DeviceState::Secondaries) {
            m_item->setLoading(true);
        } else if(state == DeviceState::Activated) {
            m_item->setChecked(true);
        } else {
            m_item->setLoading(false);
            m_item->setChecked(false);
        }
    });
    connect(m_item, &NetworkGenericListItem::clearButtonClicked,
            this, [this] {
        m_dbusNetwork->DisconnectDevice(QDBusObjectPath(path()));
    });

    NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

    connect(main_widget, &NetworkMainWidget::deviceUpdated, this, [this, main_widget] {
        const QString &vendor = this->vendor();

        if (main_widget->modemCount() > 1)
            setTitle(tr("Modem Network") + " (" + (vendor.isEmpty() ? tr("Interface") : vendor) + ")");
        else
            setTitle(tr("Modem Network"));
    });
}
