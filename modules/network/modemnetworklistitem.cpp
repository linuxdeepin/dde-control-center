#include "modemnetworklistitem.h"
#include "networkglobal.h"
#include "dbus/dbusnetwork.h"
#include "networkgenericlistitem.h"

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
            m_item->setLoading(false);
        }
    }
}

void ModemNetworkListItem::init()
{
    m_item = new NetworkGenericListItem(m_dbusNetwork);
    m_item->setTitle(tr("Restore the connect"));
    m_item->setChecked(state() == DeviceState::Activated);
    m_item->setLoading(true);

    listWidget()->addWidget(m_item);

    connect(m_item, &NetworkGenericListItem::clicked, this, [this] {
        m_dbusNetwork->ActivateConnection(uniqueUuid(), QDBusObjectPath(path()));
    });

    connect(this, &ModemNetworkListItem::uniqueUuidChanged, this, &ModemNetworkListItem::onConnectsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &ModemNetworkListItem::onConnectsChanged);
    connect(this, &ModemNetworkListItem::stateChanged, this, [this](int state) {
        m_item->setChecked(state == DeviceState::Activated);
    });
}
