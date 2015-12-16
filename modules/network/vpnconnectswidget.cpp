#include "vpnconnectswidget.h"
#include "dbus/dbusnetwork.h"
#include "networkglobal.h"
#include "networkgenericlistitem.h"

VPNConnectsWidget::VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("VPN Connections"), dbus, parent)
{
    setEnabled(dbus->vpnEnabled());

    onConnectsChanged();
    onActiveConnectionsChanged();

    connect(dbus, &DBusNetwork::VpnEnabledChanged, this, [this, dbus]{
        setEnabled(dbus->vpnEnabled());
    });
    connect(this, &VPNConnectsWidget::enabledChanged, dbus, &DBusNetwork::setVpnEnabled);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &VPNConnectsWidget::onConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &VPNConnectsWidget::onActiveConnectionsChanged);
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

    if(!item)
        return;
    if(item->checked() || item->loading())
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
                    if(item->uuid() == json_obj["Uuid"].toString()) {
                        item->setState(json_obj["State"].toInt());
                        continue;
                    }
                }
            }
        }
    }
}
