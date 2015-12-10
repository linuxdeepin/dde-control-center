#include <QDebug>

#include "constants.h"

#include "networkglobal.h"
#include "wirednetworklistitem.h"
#include "networkgenericlistitem.h"
#include "dbus/dbusnetwork.h"

WiredNetworkListItem::WiredNetworkListItem(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("Wired Network"), dbus, parent)
{
    init();
}

void WiredNetworkListItem::init()
{
    NetworkGenericListItem *item = new NetworkGenericListItem(m_dbusNetwork);

    item->setTitle(tr("Wired Connection"));

    listWidget()->addWidget(item);

    connect(this, &WiredNetworkListItem::pathChanged, this, [this, item] {
        ASYN_CALL(m_dbusNetwork->GetWiredConnectionUuid(QDBusObjectPath(path())), {
                      item->setUuid(args[0].toString());
                  }, item)
    });

    onConnectsChanged();

    auto onActiveConnectsChanged = [this] {
        const QJsonDocument &json_doc = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8());

        for(NetworkGenericListItem *item : m_mapPppoePathToItem.values()) {
            item->setState(ActiveConnectionState::Unknown);
        }

        for(const QJsonValue &value : json_doc.object()) {
            const QJsonObject &json_obj = value.toObject();

            const QJsonArray &array = json_obj["Devices"].toArray();

            if(array.toVariantList().indexOf(path()) >= 0) {
                for(NetworkGenericListItem *item : m_mapPppoePathToItem.values()) {
                    if(item->uuid() == json_obj["Uuid"].toString())
                        item->setState(json_obj["State"].toInt());
                }
            }
        }
    };

    connect(item, &NetworkGenericListItem::clicked, this, &WiredNetworkListItem::onItemClicked);
    connect(item, &NetworkGenericListItem::uuidChanged, item, onActiveConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged,
            this, &WiredNetworkListItem::onConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, item, onActiveConnectsChanged);
    connect(item, &NetworkGenericListItem::clearButtonClicked,
            this, [this] {
        m_dbusNetwork->DisconnectDevice(QDBusObjectPath(path()));
    });
    connect(this, &WiredNetworkListItem::stateChanged, item, [item](int state) {
        if(state >= DeviceState::Prepare && state <= DeviceState::Secondaries) {
            item->setLoading(true);
        } else if(state == DeviceState::Activated) {
            item->setChecked(true)  ;
        } else {
            item->setChecked(false);
            item->setLoading(false);
        }
    });
    connect(this, &WiredNetworkListItem::pathChanged, item, &NetworkGenericListItem::setDevicePath);
}

void WiredNetworkListItem::onConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    QList<NetworkGenericListItem*> tmp_list = m_mapPppoePathToItem.values();

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Pppoe].toArray()) {
        const QJsonObject &json_object = value.toObject();

        if(json_object["HwAddress"].toString().isEmpty()
                || json_object["HwAddress"].toString() == hwAddress()) {
            NetworkGenericListItem *item = m_mapPppoePathToItem.value(json_object["Path"].toString(), nullptr);

            if(!item) {
                item = new NetworkGenericListItem(m_dbusNetwork);

                listWidget()->addWidget(item);


                item->setDevicePath(path());

                connect(item, &NetworkGenericListItem::clicked,
                        this, &WiredNetworkListItem::onItemClicked);
                connect(item, &NetworkGenericListItem::clearButtonClicked,
                        this, &WiredNetworkListItem::onClearButtonClicked);
                connect(item, &NetworkGenericListItem::stateChanged,
                        this, &WiredNetworkListItem::onItemStateChanged);
                connect(this, &WiredNetworkListItem::pathChanged,
                        item, &NetworkGenericListItem::setDevicePath);

                item->updateInfoByMap(json_object.toVariantMap());

                m_mapPppoePathToItem[item->path()] = item;
            } else {
                tmp_list.removeOne(item);
                item->updateInfoByMap(json_object.toVariantMap());
            }

            item->setConnectPath(item->path());
            item->setTitle(json_object["Id"].toString());
        }
    }

    for(NetworkGenericListItem *item : tmp_list) {
        m_mapPppoePathToItem.remove(item->path());

        int index = listWidget()->indexOf(item);

        if(index < 0)
            item->deleteLater();
        else
            listWidget()->removeWidget(index);
    }
}

void WiredNetworkListItem::onItemClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(item) {
        if(item->checked())
            item->onArrowClicked();
        else
            m_dbusNetwork->ActivateConnection(item->uuid(), QDBusObjectPath(path()));
    }
}

void WiredNetworkListItem::onClearButtonClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    m_dbusNetwork->DisconnectDevice(QDBusObjectPath(path()));
}

void WiredNetworkListItem::onItemStateChanged(int state)
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
