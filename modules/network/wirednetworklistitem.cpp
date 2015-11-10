#include <QDebug>

#include "constants.h"

#include "networkglobal.h"
#include "wirednetworklistitem.h"
#include "networkgenericlistitem.h"
#include "dbus/dbusnetwork.h"

WiredNetworkListItem::WiredNetworkListItem(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("Wired Network"), dbus, parent)
{
    ///show later
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void WiredNetworkListItem::init()
{
    NetworkGenericListItem *item = new NetworkGenericListItem(m_dbusNetwork);

    item->setTitle(tr("Wired Connection"));
    item->setChecked(state() == ActiveConnectionState::Activated);

    listWidget()->addWidget(item);

    ASYN_CALL(m_dbusNetwork->GetWiredConnectionUuid(QDBusObjectPath(path())), {
                  item->setUuid(args[0].toString());
              }, item)

    onConnectsChanged();

    auto onActiveConnectsChanged = [this, item] {
        const QJsonDocument &json_doc = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8());

        for(const QJsonValue &value : json_doc.object()) {
            if(value.toObject()["Uuid"] == item->uuid()) {
                item->setChecked(value.toObject()["State"].toInt() == ActiveConnectionState::Activated);
            }
        }
    };

    connect(item, &NetworkGenericListItem::clicked, this, &WiredNetworkListItem::onItemClicked);
    connect(item, &NetworkGenericListItem::uuidChanged, item, onActiveConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &WiredNetworkListItem::onConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, item, onActiveConnectsChanged);
}

void WiredNetworkListItem::onConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Pppoe].toArray()) {
        const QJsonObject &json_object = value.toObject();

        if(json_object["HwAddress"].toString().isEmpty()
                || json_object["HwAddress"].toString() == hwAddress()) {
            NetworkGenericListItem *item = m_mapPppoePathToItem.value(json_object["Path"].toString(), nullptr);

            if(!item) {
                item = new NetworkGenericListItem(m_dbusNetwork);

                listWidget()->addWidget(item);

                m_mapPppoePathToItem[item->path()] = item;

                connect(item, &NetworkGenericListItem::clicked, this, &WiredNetworkListItem::onItemClicked);
            }

            item->updateInfoByMap(json_object.toVariantMap());
            item->setConnectPath(item->path());
            item->setTitle(json_object["Id"].toString());
        }
    }
}

void WiredNetworkListItem::onItemClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());
    if(item) {
        ASYN_CALL(m_dbusNetwork->GetWiredConnectionUuid(QDBusObjectPath(path())), {
                      item->setUuid(args[0].toString());
                      ASYN_CALL(m_dbusNetwork->ActivateConnection(item->uuid(), QDBusObjectPath(path())), {
                                    qDebug() << "ActivateConnection Reply:" << (qvariant_cast<QDBusObjectPath>(args[0]).path());
                                }, this)
                  }, item, this)
    }
}
