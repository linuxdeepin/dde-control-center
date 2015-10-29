#include "scrollframe.h"

#include "wirelessnetworklistitem.h"
#include "dbus/dbusnetwork.h"
#include "dbus/dbusconnectionsession.h"
#include "networkgenericlistitem.h"
#include "networkglobal.h"
#include "inputpassworddialog.h"
#include "connecttohiddenapwidget.h"

WirelessNetworkListItem::WirelessNetworkListItem(DBusNetwork *dbus, ScrollFrame *scrollWidget, QWidget *parent) :
    AbstractDeviceWidget(tr("Wireless Network"), dbus, scrollWidget, parent)
{
    init();
}

void WirelessNetworkListItem::onItemClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());
    if(!item || item->checked())
        return;

    if(item->uuid().isEmpty() && item->securedInEap()) {
        ASYN_CALL(m_dbusNetwork->CreateConnection(ConnectionType::Wireless, QDBusObjectPath(path())), {
                      const QDBusObjectPath &object = qvariant_cast<QDBusObjectPath>(args[0]);

                      qDebug() << "connect to hidden access point:" << object.path();

                      DBusConnectionSession *dbus = new DBusConnectionSession(object.path());
                      ConnectToHiddenApWidget *connect_ap = new ConnectToHiddenApWidget(dbus);

                      m_scrollWidget->pushWidget(connect_ap);

                      connect(connect_ap, &ConnectToHiddenApWidget::cancel,
                              m_scrollWidget, &ScrollFrame::popCurrentWidget);
                      connect(connect_ap, &ConnectToHiddenApWidget::confirm,
                              m_scrollWidget, &ScrollFrame::popCurrentWidget);
                  }, this)
    } else {
        ASYN_CALL(m_dbusNetwork->ActivateAccessPoint(item->uuid(),
                                                     QDBusObjectPath(item->path()),
                                                     QDBusObjectPath(path())), {
                      const QString &connect_path = qvariant_cast<QDBusObjectPath>(args[0]).path();
                      if(!connect_path.isEmpty())
                          item->setConnectPath(connect_path);
                      qDebug() << "ActivateAccessPoint:" << connect_path;
                  }, this, item)
    }
}

void WirelessNetworkListItem::onDbusConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    for(const QJsonValue &value : json_doc.object()[ConnectionType::Wireless].toArray()) {
        const QJsonObject &json_object = value.toObject();

        if(json_object["HwAddress"].toString() == hwAddress()
                || json_object["HwAddress"].toString().isEmpty()) {
            NetworkGenericListItem *item = m_mapApSsidToItem.value(json_object["Ssid"].toString(), nullptr);

            if(item) {
                item->setUuid(json_object["Uuid"].toString());
                item->setConnectPath(json_object["Path"].toString());
            }
        }
    }
}

void WirelessNetworkListItem::updateItemIndex(int strength)
{
    /// on wifi strength changed
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());
    int index = listWidget()->indexOf(item);

    for(int i = 0; i < listWidget()->count(); ++i){
        NetworkGenericListItem *tmp_item = qobject_cast<NetworkGenericListItem*>(listWidget()->getWidget(i));

        if(tmp_item && tmp_item->strength() < strength) {
            if(index == i - 1)
                return;

            if(index >= 0)
                listWidget()->removeWidget(index, false);

            if(index < i && index >= 0)
                listWidget()->insertWidget(i - 1, item);
            else
                listWidget()->insertWidget(i, item);

            return;
        }
    }
}

NetworkGenericListItem *WirelessNetworkListItem::addAccessPoint(const QVariantMap &map)
{
    NetworkGenericListItem *item = m_mapApSsidToItem.value(map["Ssid"].toString(), nullptr);

    if(!item) {
        item = new NetworkGenericListItem(m_dbusNetwork);
        item->updateInfoByMap(map);
        m_mapApSsidToItem[item->ssid()] = item;
        listWidget()->addWidget(item);
        connect(item, &NetworkGenericListItem::strengthChanged, this, &WirelessNetworkListItem::updateItemIndex);
        connect(item, &NetworkGenericListItem::clicked, this, &WirelessNetworkListItem::onItemClicked);
    } else if(item->strength() < map["Strength"].toInt()) {
        item->updateInfoByMap(map);
    }

    if(activeAp() == item->path()) {
        item->setChecked(true);
        m_activeItem = item;
    }

    m_mapApPathToItem[item->path()] = item;

    return item;
}

void WirelessNetworkListItem::init()
{
    connect(this, &WirelessNetworkListItem::pathChanged,
            this, [this](const QString &path){
        ASYN_CALL(m_dbusNetwork->GetAccessPoints(QDBusObjectPath(path)), {
                      listWidget()->clear();

                      QJsonDocument json_doc = QJsonDocument::fromJson(args[0].toByteArray());
                      QVariantList map_list = json_doc.array().toVariantList();
                      qSort(map_list.begin(), map_list.end(), [](const QVariant &v1, const QVariant &v2)->bool{
                          return v1.toMap()["Strength"].toInt() > v2.toMap()["Strength"].toInt();
                      });
                      for(const QVariant &map : map_list) {
                          addAccessPoint(map.toMap());
                      }

                      NetworkGenericListItem *hidden_ap = new NetworkGenericListItem(m_dbusNetwork);
                      hidden_ap->setTitle(tr("Connect to hidden access point"));
                      listWidget()->addWidget(hidden_ap);

                      connect(hidden_ap, &NetworkGenericListItem::clicked,
                              this, &WirelessNetworkListItem::onItemClicked);

                      onDbusConnectsChanged();
                  }, this)
    });

    connect(m_dbusNetwork, &DBusNetwork::AccessPointAdded,
            this, [this](const QString &path, const QString &info){
        if(path == this->path()) {
            QJsonDocument json_doc = QJsonDocument::fromJson(info.toUtf8());
            const QVariantMap &map = json_doc.object().toVariantMap();

            NetworkGenericListItem *item = addAccessPoint(map);
            item->strengthChanged(item->strength());

            qDebug() << "add access point:" << item->path() << item->ssid();
        }
    });

    connect(m_dbusNetwork, &DBusNetwork::AccessPointRemoved, this, [this](const QString &path, const QString &info){
        if(path == this->path()) {
            QJsonDocument json_doc = QJsonDocument::fromJson(info.toUtf8());
            const QString &ap_path = json_doc.object().toVariantMap().value("Path").toString();
            if(ap_path.isEmpty())
                return;

            NetworkGenericListItem *item = m_mapApPathToItem.value(ap_path, nullptr);
            if(item) {
                int index = listWidget()->indexOf(item);
                m_mapApPathToItem.remove(ap_path);
                m_mapApSsidToItem.remove(item->ssid());
                if(index >= 0)
                    listWidget()->removeWidget(index);
            }

            qDebug() << "remove access point:" << ap_path << json_doc.object().toVariantMap()["Ssid"];
        }
    });

    connect(this, &WirelessNetworkListItem::activeApChanged, this, [this](const QString &path){
        qDebug() << "activeApChanged:" << path;

        if(m_activeItem) {
            m_activeItem->setChecked(false);
            m_activeItem = nullptr;
        }

        NetworkGenericListItem *item = m_mapApPathToItem.value(path, nullptr);
        if(item) {
            item->setChecked(true);
            m_activeItem = item;
        }
    });

    connect(m_dbusNetwork, &DBusNetwork::AccessPointPropertiesChanged, this, [this](const QString &path, const QString &info){
        if(path == this->path()) {
            QJsonDocument json_doc = QJsonDocument::fromJson(info.toUtf8());
            const QVariantMap &map = json_doc.object().toVariantMap();
            NetworkGenericListItem *item = m_mapApPathToItem.value(map["Path"].toString(), nullptr);
            if(item)
                item->updateInfoByMap(map);
        }
    });

    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &WirelessNetworkListItem::onDbusConnectsChanged);

    connect(m_dbusNetwork, &DBusNetwork::NeedSecrets,
            this, [this](const QString &in0, const QString &in1, const QString &in2, bool in3){
        qDebug() << in0 << in1 << in2 << in3;
        if(!m_inputPasswording) {
            NetworkGenericListItem *item = m_mapApSsidToItem.value(in2, nullptr);

            if(item)
                qDebug() << item->connectPath() << in0 << item->ssid();

            if(!item || item->connectPath() != in0)
                return;

            int index = listWidget()->indexOf(item);

            if(index < 0)
                return;

            m_inputPasswording = true;

            for(int i = 0; i < listWidget()->count(); ++i)
                listWidget()->getWidget(i)->setEnabled(false);

            InputPasswordDialog dialog;
            connect(&dialog, &InputPasswordDialog::textChanged, this, [&dialog]{dialog.setInputAlert(false);});
            listWidget()->insertWidget(index + 1, &dialog);

            execDialog:

            if(dialog.exec()) {
                if(!dialog.text().isEmpty()) {
                    m_dbusNetwork->FeedSecret(in0, in1, dialog.text(), dialog.autoConnect());
                } else {
                    dialog.setInputAlert(true);
                    goto execDialog;
                }
            } else {
                m_dbusNetwork->CancelSecret(in0, in1);
            }

            listWidget()->removeWidget(listWidget()->indexOf(&dialog));

            for(int i = 0; i < listWidget()->count(); ++i)
                listWidget()->getWidget(i)->setEnabled(true);

            m_inputPasswording = false;
        }
    });
}
