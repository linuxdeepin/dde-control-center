#include "scrollframe.h"

#include "wirelessnetworklistitem.h"
#include "dbus/dbusnetwork.h"
#include "dbus/dbusconnectionsession.h"
#include "networkgenericlistitem.h"
#include "networkglobal.h"
#include "inputpassworddialog.h"
#include "connecttohiddenappage.h"
#include "networkmainwidget.h"
#include "editconnectionpage.h"

WirelessNetworkListItem::WirelessNetworkListItem(DBusNetwork *dbus, QWidget *parent) :
    AbstractDeviceWidget(tr("Wireless Network"), dbus, parent)
{
    init();
}

void WirelessNetworkListItem::onItemClicked()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    if(item->checked()) {
        item->onArrowClicked();
        return;
    }

    if(item->uuid().isEmpty() && item->securedInEap()) {
        ASYN_CALL(m_dbusNetwork->CreateConnection(ConnectionType::Wireless, QDBusObjectPath(path())), {
                      const QDBusObjectPath &object = qvariant_cast<QDBusObjectPath>(args[0]);

                      qDebug() << "connect to hidden access point:" << object.path();

                      DBusConnectionSession *dbus = new DBusConnectionSession(object.path(), this);
                      ConnectToHiddenApPage *connect_ap = new ConnectToHiddenApPage(dbus);

                      ScrollFrame *scrollWidget = DCCNetwork::parentNetworkMainWidget(this);

                      scrollWidget->pushWidget(connect_ap);

                      connect(connect_ap, &ConnectToHiddenApPage::cancel,
                              scrollWidget, &ScrollFrame::popCurrentWidget);
                      connect(connect_ap, &ConnectToHiddenApPage::confirm,
                              scrollWidget, &ScrollFrame::popCurrentWidget);
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

void WirelessNetworkListItem::onConnectsChanged()
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
                m_mapApUuidToItem[item->uuid()] = item;
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

void WirelessNetworkListItem::updateActiveApState()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    if(item->state() == ActiveConnectionState::Activating) {
        item->setLoading(true);
        m_activeItem = item;
    } else if(item->state() == ActiveConnectionState::Activated) {
        item->setChecked(true);
        m_activeItem = item;
    } else {
        item->setChecked(false);
        item->setLoading(false);
    }
}

void WirelessNetworkListItem::onActiveConnectionsChanged()
{
    if(m_activeItem) {
        m_activeItem->setState(ActiveConnectionState::Unknown);
    }

    const QJsonDocument &json_doc = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8());

    for(const QJsonValue &value : json_doc.object()) {
        const QJsonObject &json_obj = value.toObject();

        if(json_obj["Devices"].toArray().toVariantList().indexOf(path()) >= 0) {
            NetworkGenericListItem *item = m_mapApUuidToItem.value(json_obj["Uuid"].toString(), nullptr);

            if(item) {
                item->setState(json_obj["State"].toInt());
            }
        }
    }
}

NetworkGenericListItem *WirelessNetworkListItem::addAccessPoint(const QVariantMap &map)
{
    NetworkGenericListItem *item = m_mapApSsidToItem.value(map["Ssid"].toString(), nullptr);

    if(!item) {
        item = new NetworkGenericListItem(m_dbusNetwork);
        item->updateInfoByMap(map);
        item->setDevicePath(path());

        m_mapApSsidToItem[item->ssid()] = item;

        listWidget()->addWidget(item);

        connect(item, &NetworkGenericListItem::strengthChanged,
                this, &WirelessNetworkListItem::updateItemIndex);
        connect(item, &NetworkGenericListItem::clicked, this, &WirelessNetworkListItem::onItemClicked);
        connect(item, &NetworkGenericListItem::clearButtonClicked,
                this, [this] {
            m_dbusNetwork->DisconnectDevice(QDBusObjectPath(path()));
        });
        connect(item, &NetworkGenericListItem::stateChanged, this, &WirelessNetworkListItem::updateActiveApState);
        connect(this, &WirelessNetworkListItem::pathChanged, item, &NetworkGenericListItem::setDevicePath);
        item->disconnect(item, SIGNAL(rightArrowClicked()), item, SLOT(onArrowClicked()));
        /// 改动此处代码时需注意：此代码的目的是不让右边箭头点击后调用NetworkGenericListItem::onArrowClicked
        connect(item, &NetworkGenericListItem::rightArrowClicked, this, [item, this] {
            if(item->uuid().isEmpty()) {
                NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

                if(main_widget) {
                    const QDBusObjectPath &path = m_dbusNetwork->CreateConnectionForAccessPoint(QDBusObjectPath(item->path()),
                                                                                                QDBusObjectPath(this->path()));

                    if(path.path().isEmpty())
                        return;

                    main_widget->pushWidget(new EditConnectionPage(path.path()));
                }
            } else {
                item->onArrowClicked();
            }
        });
    } else if(item->strength() < map["Strength"].toInt() || map["Path"] == activeAp()) {
        item->updateInfoByMap(map);
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
                      connect(hidden_ap, &NetworkGenericListItem::rightArrowClicked,
                              this, &WirelessNetworkListItem::onItemClicked);

                      onConnectsChanged();
                      onActiveConnectionsChanged();
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

                if(index < 0)
                    item->deleteLater();
                else
                    listWidget()->removeWidget(index);
            }

            qDebug() << "remove access point:" << ap_path << json_doc.object().toVariantMap()["Ssid"];
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

    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &WirelessNetworkListItem::onConnectsChanged);

    connect(m_dbusNetwork, &DBusNetwork::NeedSecrets,
            this, [this](const QString &in0, const QString &in1, const QString &in2, bool in3){

        Q_UNUSED(in3)

        if(!m_inputPasswording) {
            NetworkGenericListItem *item = m_mapApSsidToItem.value(in2, nullptr);

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

    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged,
            this, &WirelessNetworkListItem::onActiveConnectionsChanged);
}
