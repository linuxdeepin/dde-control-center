/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "vpnconnectswidget.h"
#include "dbus/dbusnetwork.h"
#include "networkglobal.h"
#include "networkgenericlistitem.h"
#include "inputpassworddialog.h"

VPNConnectsWidget::VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("VPN Connections"), dbus, parent)
{
    setEnabled(dbus->vpnEnabled());

    onConnectsChanged();
    onActiveConnectionsChanged();

    connect(dbus, &DBusNetwork::VpnEnabledChanged, this, [this, dbus]{
        setEnabled(dbus->vpnEnabled());
    });
    connect(this, &VPNConnectsWidget::enabledChanged,
            dbus, &DBusNetwork::setVpnEnabled);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged,
            this, &VPNConnectsWidget::onConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged,
            this, &VPNConnectsWidget::onActiveConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::NeedSecrets,
            this, &VPNConnectsWidget::onNeedSecrets);
}

QString VPNConnectsWidget::path() const
{
    return "/";
}

void VPNConnectsWidget::onConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    QList<NetworkGenericListItem*> tmp_list = m_mapVpnPathToItem.values();

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Vpn].toArray()) {
        const QJsonObject &json_object = value.toObject();

        NetworkGenericListItem *item = m_mapVpnPathToItem.value(json_object["Path"].toString(), nullptr);

        if(!item) {
            item = new NetworkGenericListItem(m_dbusNetwork);

            listWidget()->addWidget(item);

            connect(item, &NetworkGenericListItem::clicked, this, &VPNConnectsWidget::onItemClicked);
            connect(item, &NetworkGenericListItem::clearButtonClicked, this, &VPNConnectsWidget::onClearButtonClicked);
            connect(item, &NetworkGenericListItem::stateChanged, this, &VPNConnectsWidget::onItemStateChanged);
            connect(this, &VPNConnectsWidget::pathChanged, item, &NetworkGenericListItem::setDevicePath);

            item->setDevicePath(path());
            item->updateInfoByMap(json_object.toVariantMap());

            m_mapVpnPathToItem[item->path()] = item;
        } else {
            tmp_list.removeOne(item);
            item->updateInfoByMap(json_object.toVariantMap());
        }

        item->setConnectPath(item->path());
        item->setTitle(json_object["Id"].toString());
    }

    for(NetworkGenericListItem *item : tmp_list) {
        m_mapVpnPathToItem.remove(item->path());

        int index = listWidget()->indexOf(item);

        if(index < 0)
            item->deleteLater();
        else
            listWidget()->removeWidget(index);
    }
}

void VPNConnectsWidget::onItemClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item || item->loading())
        return;

    if(item->checked())
        item->onArrowClicked();
    else
        m_dbusNetwork->ActivateConnection(item->uuid(), QDBusObjectPath(path()));
}

void VPNConnectsWidget::onClearButtonClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    m_dbusNetwork->DeactivateConnection(item->uuid());
}

void VPNConnectsWidget::onItemStateChanged(int state)
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    if(state == ActiveConnectionState::Activating) {
        item->setLoading(true);
    } else if(state == ActiveConnectionState::Activated) {
        item->setChecked(true)  ;
    } else {
        item->setChecked(false);
        item->setLoading(false);
    }
}

void VPNConnectsWidget::onActiveConnectionsChanged()
{
    const QJsonDocument &json_doc = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8());

    for(NetworkGenericListItem *item : m_mapVpnPathToItem.values()) {
        item->setState(ActiveConnectionState::Unknown);
    }

    for(const QJsonValue &value : json_doc.object()) {
        const QJsonObject &json_obj = value.toObject();

        if(json_obj["Vpn"].toBool()) {
            for(NetworkGenericListItem *item : m_mapVpnPathToItem.values()) {
                if(item->uuid() == json_obj["Uuid"].toString()) {
                    item->setState(json_obj["State"].toInt());
                    break;
                }
            }
        }
    }
}

void VPNConnectsWidget::onNeedSecrets(const QString &path, const QString &section,
                                      const QString &ssid, bool autoConnect)
{
    Q_UNUSED(ssid)

    NetworkGenericListItem *item = m_mapVpnPathToItem.value(path, nullptr);

    m_targetConnectPath = path;
    m_targetConnectSection = section;

    if(!m_ddialog) {
        for(int i = 0; i < listWidget()->count(); ++i)
            listWidget()->getWidget(i)->setEnabled(false);

        m_ddialog = new InputPasswordDialog;

        connect(m_ddialog, &InputPasswordDialog::textChanged, this, [this] {
            m_ddialog->setInputAlert(false);
        });
        connect(m_ddialog, &InputPasswordDialog::confirm, this, [this] {
            if(!m_ddialog->text().isEmpty()) {
                m_dbusNetwork->FeedSecret(m_targetConnectPath, m_targetConnectSection,
                                          m_ddialog->text(), m_ddialog->autoConnect());
                closeInputDialog();
            } else {
                m_ddialog->setInputAlert(true);
            }
        });
        connect(m_ddialog.data(), &InputPasswordDialog::cancel, this, [this] {
            m_dbusNetwork->CancelSecret(m_targetConnectPath, m_targetConnectSection);

            closeInputDialog();
        });
    }

    m_ddialog->setAutoConnect(autoConnect);

    int index = listWidget()->indexOf(item);

    if(index < 0)
        return;

    int index_dialog = listWidget()->indexOf(m_ddialog.data());

    if(index_dialog >= 0)
        listWidget()->removeWidget(index_dialog, false);
    listWidget()->insertWidget(index + 1, m_ddialog.data());
}

void VPNConnectsWidget::closeInputDialog()
{
    if(m_ddialog.isNull())
        return;

    int index_dialog = listWidget()->indexOf(m_ddialog.data());

    if(index_dialog >= 0)
        listWidget()->removeWidget(index_dialog);
    else
        m_ddialog->deleteLater();

    for(int i = 0; i < listWidget()->count(); ++i)
        listWidget()->getWidget(i)->setEnabled(true);
}
