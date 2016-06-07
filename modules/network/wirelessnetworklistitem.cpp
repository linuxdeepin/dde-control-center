/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QPointer>

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

WirelessNetworkListItem::~WirelessNetworkListItem()
{
    m_ddialog->deleteLater();
}

void WirelessNetworkListItem::onItemClicked()
{
    QPointer<NetworkGenericListItem> item = qobject_cast<NetworkGenericListItem*>(sender());

    /// 此处必须使用智能指针，因为下面的函数都是使用异步方式调用的，有可能在函数执行时item已经被delete。

    if(!item || item->loading())
        return;

    if(item->checked()) {
        item->onArrowClicked();

        return;
    }

    if(item->uuid().isEmpty() && item->securedInEap()) {
        ScrollFrame *scrollWidget = DCCNetwork::parentNetworkMainWidget(this);

        if(scrollWidget->stackWidget()->busy())
            return;

        ASYN_CALL(m_dbusNetwork->CreateConnection(ConnectionType::Wireless, QDBusObjectPath(path())), {
                      const QDBusObjectPath &object = qvariant_cast<QDBusObjectPath>(args[0]);

                      qDebug() << "connect to hidden access point:" << object.path();

                      DBusConnectionSession *dbus = new DBusConnectionSession(object.path(), this);

                      if(scrollWidget->stackWidget()->busy()) {
                          dbus->Close().waitForFinished();
                          dbus->deleteLater();
                          return;
                      }

                      ConnectToHiddenApPage *connect_ap = new ConnectToHiddenApPage(dbus);

                      scrollWidget->pushWidget(connect_ap);

                      connect(connect_ap, &ConnectToHiddenApPage::cancel,
                              scrollWidget, &ScrollFrame::popCurrentWidget);
                      connect(connect_ap, &ConnectToHiddenApPage::confirm,
                              scrollWidget, &ScrollFrame::popCurrentWidget);
                  }, this, scrollWidget)
    } else {
        qDebug() << "item clicked, to connect: item path=" << item->path() << "item uuid=" << item->uuid();

        ASYN_CALL(m_dbusNetwork->ActivateAccessPoint(item->uuid(),
                                                     QDBusObjectPath(item->path()),
                                                     QDBusObjectPath(path())), {
                      const QString &connect_path = qvariant_cast<QDBusObjectPath>(args[0]).path();
                      if(!connect_path.isEmpty() && item)
                          item->setConnectPath(connect_path);
                      qDebug() << "ActivateAccessPoint:" << connect_path << "<<<item is null:" << item.isNull();
                  }, this, item)
    }
}

void WirelessNetworkListItem::onConnectsChanged()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->connections().toUtf8());

    connectsArray = json_doc.object()[ConnectionType::Wireless].toArray();

    updateAllItemUuid();
}

void WirelessNetworkListItem::updateItemIndex()
{
    /// on wifi strength changed
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    updateItemIndex(item);
}

void WirelessNetworkListItem::updateActiveAp()
{
    NetworkGenericListItem *item = qobject_cast<NetworkGenericListItem*>(sender());

    if(!item)
        return;

    if(item->state() == ActiveConnectionState::Activating) {
        item->setLoading(true);

        NetworkGenericListItem *oldActiveItem = m_activeItem.data();

        m_activeItem = item;

        updateItemIndex(oldActiveItem);
    } else if(item->state() == ActiveConnectionState::Activated) {
        item->setChecked(true);

        NetworkGenericListItem *oldActiveItem = m_activeItem.data();

        m_activeItem = item;

        updateItemIndex(oldActiveItem);
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

NetworkGenericListItem *WirelessNetworkListItem::addAccessPoint(const QVariantMap &map, bool auto_sort)
{
    NetworkGenericListItem *item = m_mapApSsidToItem.value(map["Ssid"].toString(), nullptr);

    if(!item) {
        item = new NetworkGenericListItem(m_dbusNetwork);
        item->updateInfoByMap(map);
        item->setDevicePath(path());

        updateItemUuid(item);

        m_mapApSsidToItem[item->ssid()] = item;

        if(auto_sort) {
            updateItemIndex(item);
        } else {
            listWidget()->addWidget(item);
        }


        connect(item, &NetworkGenericListItem::strengthChanged,
                this, static_cast<void (WirelessNetworkListItem::*)()>(&WirelessNetworkListItem::updateItemIndex));
        connect(item, &NetworkGenericListItem::clicked, this, &WirelessNetworkListItem::onItemClicked);
        connect(item, &NetworkGenericListItem::clearButtonClicked,
                this, [this] {
            m_dbusNetwork->DisconnectDevice(QDBusObjectPath(path()));
        });
        connect(item, &NetworkGenericListItem::stateChanged, this, &WirelessNetworkListItem::updateActiveAp);
        connect(this, &WirelessNetworkListItem::pathChanged, item, &NetworkGenericListItem::setDevicePath);
        item->disconnect(item, SIGNAL(rightArrowClicked()), item, SLOT(onArrowClicked()));
        /// 改动此处代码时需注意：此代码的目的是不让右边箭头点击后调用NetworkGenericListItem::onArrowClicked
        connect(item, &NetworkGenericListItem::rightArrowClicked, this, [item, this] {
            if(item->uuid().isEmpty()) {
                NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

                if(main_widget && !main_widget->stackWidget()->busy()) {
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

        if(item->path() == activeAp()) {
            onActiveConnectionsChanged();
            /// update item state.
        }
    } else if(item->strength() < map["Strength"].toInt() || map["Path"] == activeAp()) {
        m_mapApPathToItem.remove(item->path());

        item->updateInfoByMap(map);
        /// 注意，此处更新会更新item的path，所以要记得先删除path map中旧的path项
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

            NetworkGenericListItem *item = addAccessPoint(map, true);
            //item->strengthChanged(item->strength());

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

            const QString item_ssid = json_doc.object()["Ssid"].toString();

            if(item) {
                int index = listWidget()->indexOf(item);

                m_mapApSsidToItem.remove(item_ssid);
                m_mapApPathToItem.remove(ap_path);
                m_mapApUuidToItem.remove(item->uuid());

                qDebug() << "remove item:" << ap_path << item_ssid << ",ListWidget.indexOf=" << index;
                qDebug() << "remove target:" << item;

                if(index < 0)
                    item->deleteLater();
                else
                    listWidget()->removeWidget(index);
            }

            qDebug() << "remove access point:" << ap_path << item_ssid;
        }
    });

    connect(m_dbusNetwork, &DBusNetwork::AccessPointPropertiesChanged, this, [this](const QString &path, const QString &info){
        if(path == this->path()) {
            QJsonDocument json_doc = QJsonDocument::fromJson(info.toUtf8());
            const QVariantMap &map = json_doc.object().toVariantMap();
            NetworkGenericListItem *item = m_mapApPathToItem.value(map["Path"].toString(), nullptr);

            if(item) {
                m_mapApSsidToItem.remove(item->ssid());

                item->updateInfoByMap(map);
                /// 注意，此处调用可能会更新ssid的值，所以也要更新map ssid

                m_mapApSsidToItem[item->ssid()] = item;
            }
        }
    });

    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &WirelessNetworkListItem::onConnectsChanged);

    connect(m_dbusNetwork, &DBusNetwork::NeedSecrets,
            this, [this](const QString &path, const QString &section, const QString &ssid, bool autoConnect){

        NetworkGenericListItem *item = m_mapApSsidToItem.value(ssid, nullptr);

        if(!item || item->connectPath() != path)
            return;

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
    });
    connect(m_dbusNetwork, &DBusNetwork::NeedSecretsFinished,
            this, [this](const QString &connPath, const QString &section) {
        if(m_ddialog && m_targetConnectPath == connPath
                && m_targetConnectSection == section) {
            closeInputDialog();
        }
    });
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged,
            this, &WirelessNetworkListItem::onActiveConnectionsChanged);

    NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

    connect(main_widget, &NetworkMainWidget::deviceUpdated, this, [this, main_widget] {
        const QString &vendor = this->vendor();

        if (main_widget->wirelessCount() > 1)
            setTitle(tr("Wireless Network") + " (" + (vendor.isEmpty() ? tr("Interface") : vendor) + ")");
        else
            setTitle(tr("Wireless Network"));
    });
}

void WirelessNetworkListItem::updateItemIndex(NetworkGenericListItem *item)
{
    if(!item || (m_ddialog && item == m_activeItem.data()))
        return;

    if(m_ddialog && m_targetConnectPath == item->connectPath())
        return;

    int index = listWidget()->indexOf(item);
    int strength = item->strength();

    for(int i = 0; i < listWidget()->count(); ++i){
        NetworkGenericListItem *tmp_item = qobject_cast<NetworkGenericListItem*>(listWidget()->getWidget(i));

        if(tmp_item && tmp_item->strength() < strength) {
            if(index >= 0) {
                if(index == i - 1)
                    return;

                listWidget()->removeWidget(index, false);
            }

            if(index < i && index >= 0)
                listWidget()->insertWidget(i - 1, item);
            else
                listWidget()->insertWidget(i, item);

            return;
        }
    }
}

void WirelessNetworkListItem::closeInputDialog()
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

    updateItemIndex(m_activeItem.data());
}

void WirelessNetworkListItem::updateAllItemUuid()
{
    for(NetworkGenericListItem *item : m_mapApSsidToItem.values()) {
        m_mapApUuidToItem.remove(item->uuid());
        /// 注意：此处必须要先移除旧的item uuid。

        item->setUuid("");
    }

    for(const QJsonValue &value : connectsArray) {
        const QJsonObject &connectsObject = value.toObject();

        if(connectsObject["HwAddress"].toString() == hwAddress()
                || connectsObject["HwAddress"].toString().isEmpty()) {
            NetworkGenericListItem *item = m_mapApSsidToItem.value(connectsObject["Ssid"].toString(), nullptr);

            if(item) {
                item->setUuid(connectsObject["Uuid"].toString());
                item->setConnectPath(connectsObject["Path"].toString());
                m_mapApUuidToItem[item->uuid()] = item;
            }
        }
    }
}

void WirelessNetworkListItem::updateItemUuid(NetworkGenericListItem *item)
{
    m_mapApUuidToItem.remove(item->uuid());
    /// 注意：此处必须要先移除旧的item uuid。

    item->setUuid("");

    for(const QJsonValue &value : connectsArray) {
        const QJsonObject &connectsObject = value.toObject();

        if(connectsObject["Ssid"].toString() == item->ssid()
                &&(connectsObject["HwAddress"].toString() == hwAddress()
                || connectsObject["HwAddress"].toString().isEmpty())) {
            item->setUuid(connectsObject["Uuid"].toString());
            item->setConnectPath(connectsObject["Path"].toString());
            m_mapApUuidToItem[item->uuid()] = item;
        }
    }
}
