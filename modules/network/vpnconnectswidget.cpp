#include "vpnconnectswidget.h"
#include "dbus/dbusnetwork.h"
#include "networkglobal.h"
#include "networkgenericlistitem.h"

VPNConnectsWidget::VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("VPN Connections"), dbus, parent)
{
    setEnabled(dbus->vpnEnabled());

    onConnectsChanged();

    connect(dbus, &DBusNetwork::VpnEnabledChanged, this, [this, dbus]{
        setEnabled(dbus->vpnEnabled());
    });
    connect(this, &VPNConnectsWidget::enabledChanged, dbus, &DBusNetwork::setVpnEnabled);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &VPNConnectsWidget::onConnectsChanged);
}

void VPNConnectsWidget::onConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Vpn].toArray()) {
        const QJsonObject &json_object = value.toObject();

        NetworkGenericListItem *item = m_mapVpnPathToItem.value(json_object["Path"].toString(), nullptr);
        if(!item) {
            item = new NetworkGenericListItem(m_dbusNetwork);

            listWidget()->addWidget(item);

            m_mapVpnPathToItem[item->path()] = item;
        }

        item->updateInfoByMap(json_object.toVariantMap());
        item->setConnectPath(item->path());
        item->setTitle(json_object["Id"].toString());
    }
}

