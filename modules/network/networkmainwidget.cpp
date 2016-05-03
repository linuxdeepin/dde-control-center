/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QVBoxLayout>

#include <dthememanager.h>
#include <dwidget_global.h>
#include <dseparatorhorizontal.h>
#include <dswitchbutton.h>
#include <dtextbutton.h>
#include <dheaderline.h>
#include <dlistwidget.h>
#include <darrowlineexpand.h>

#include "moduleheader.h"
#include "constants.h"
#include "normallabel.h"
#include "imagenamebutton.h"
#include "genericlistitem.h"
#include "generaladdbutton.h"
#include "dynamiclabel.h"

#include "wirednetworklistitem.h"
#include "wirelessnetworklistitem.h"
#include "modemnetworklistitem.h"
#include "vpnconnectswidget.h"
#include "networkmainwidget.h"
#include "addconnectpage.h"
#include "systemproxywidget.h"
#include "networkinfo.h"
#include "dbus/dbusdccnetworkservice.h"
#include "network.h"
#include "controlcenterproxyinterface.h"

namespace DCCNetwork {
    NetworkMainWidget *parentNetworkMainWidget(const QObject *obj)
    {
        return parentObject<NetworkMainWidget>(obj);
    }
}

NetworkMainWidget::NetworkMainWidget(Network *network, QWidget *parent) :
    ScrollFrame(parent),
    m_networkModule(network)
{
    m_dbusNetwork = new DBusNetwork(this);

    initUI();
    updateUI();

    DBusDCCNetworkService *service = new DBusDCCNetworkService(this);

    Q_UNUSED(service);

    QDBusConnection::sessionBus().registerObject("/com/deepin/dde/ControlCenter/Network", this);

    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &NetworkMainWidget::updateUI);
    connect(network, &Network::dccVisibleChanged, this, &NetworkMainWidget::dccVisibleChanged);
}

DBusNetwork *NetworkMainWidget::dbusNetwork() const
{
    return m_dbusNetwork;
}

bool NetworkMainWidget::dccIsVisible() const
{
    return m_networkModule->getInterface()->isVisible();
}

void NetworkMainWidget::updateDeviceByMap(const QString &type, const QVariantMap &map,
                                          int index, QList<AbstractDeviceWidget *> &tmp_list)
{
    const QString &path = map["Path"].toString();
    AbstractDeviceWidget *item = m_mapPathToObject.value(path, nullptr);

    if(item) {
        item->updateInfoByMap(map);
        tmp_list.removeOne(item);
    } else {

        if(type == DeviceType::Wired) {
            item = new WiredNetworkListItem(m_dbusNetwork, this);
        } else if(type == DeviceType::Wireless){
            item = new WirelessNetworkListItem(m_dbusNetwork, this);
        } else if(type == DeviceType::Modem) {
            item = new ModemNetworkListItem(m_dbusNetwork, this);
        } else {
            return;
        }

        item->updateInfoByMap(map);

        m_mapPathToObject[path] = item;

        mainLayout()->insertWidget(index, item);

        qDebug() << "new device: type = " << type << path << map["Vendor"];
    }
}

void NetworkMainWidget::updateUI()
{
    QJsonDocument json_doc = QJsonDocument::fromJson(m_dbusNetwork->devices().toUtf8());
    QJsonObject json_obj = json_doc.object();

    QList<AbstractDeviceWidget*> tmp_list = m_mapPathToObject.values();

    m_wiredCount = m_wirelessCount = m_modemCount = 0;

    for(const QJsonValue &value : json_obj[DeviceType::Wired].toArray()) {
        const QVariantMap &map = value.toObject().toVariantMap();
        if(map["State"].toInt() == DeviceState::Unavailable
                && map["Vendor"].toString().toLower() == "apply")
            continue;

        ++m_wiredCount;

        updateDeviceByMap(DeviceType::Wired, map, m_wiredCount - 1, tmp_list);
    }

    for(const QJsonValue &value : json_obj[DeviceType::Modem].toArray()) {
        ++m_modemCount;

        const QVariantMap &map = value.toObject().toVariantMap();

        updateDeviceByMap(DeviceType::Modem, map, m_wiredCount + m_modemCount - 1, tmp_list);
    }

    for(const QJsonValue &value : json_obj[DeviceType::Wireless].toArray()) {
        ++m_wirelessCount;

        const QVariantMap &map = value.toObject().toVariantMap();

        updateDeviceByMap(DeviceType::Wireless, map,
                          m_wiredCount + m_modemCount + m_wirelessCount - 1, tmp_list);
    }

    for(AbstractDeviceWidget *widget : tmp_list) {
        m_mapPathToObject.remove(widget->path());
        widget->deleteLater();
    }
}

void NetworkMainWidget::initUI()
{
    setFixedWidth(DCC::ModuleContentWidth);

    ModuleHeader *header = new ModuleHeader(tr("Network"), false);
    QWidget *header_right_widget = new QWidget;
    QHBoxLayout *header_right_widget_layout = new QHBoxLayout(header_right_widget);
    GeneralAddButton *add_button = new GeneralAddButton;
    ImageNameButton *info_button = new ImageNameButton("info");
    DynamicLabel* info_dynamicLabel = new DynamicLabel(this);
    info_dynamicLabel->move(95, 12);
    info_dynamicLabel->setText(tr("View detailed information"));

    header->setFixedSize(DCC::ModuleContentWidth, DTK_WIDGET_NAMESPACE::CONTENT_HEADER_HEIGHT);
    header_right_widget_layout->setMargin(0);
    header_right_widget_layout->setSpacing(5);
    header_right_widget_layout->addWidget(add_button);
    header_right_widget_layout->addWidget(info_button);
    header->setRightContent(header_right_widget);

    headerLayout()->addWidget(header);
    headerLayout()->addWidget(new DSeparatorHorizontal);

    m_vpnConnectsWidget = new VPNConnectsWidget(m_dbusNetwork, this);

    DArrowLineExpand *expand_proxy = new DArrowLineExpand;
    SystemProxyWidget *widget_proxy = new SystemProxyWidget(m_dbusNetwork);

    expand_proxy->setTitle(tr("System Proxy"));
    expand_proxy->setContent(widget_proxy);

    mainLayout()->addWidget(m_vpnConnectsWidget);
    mainLayout()->addWidget(expand_proxy);

    connect(this, &NetworkMainWidget::currentMainWidgetChanged,
            this, [header_right_widget, this](const QWidget *w){
        header_right_widget->setVisible(w == mainLayout()->parentWidget());
    });
    connect(add_button, &GeneralAddButton::clicked, this, [this] {
        AddConnectPage *add_connect_page = new AddConnectPage;

        if(!stackWidget()->busy())
            pushWidget(add_connect_page);
    });
    connect(info_button, &ImageNameButton::clicked, this, [this] {
        if(!stackWidget()->busy())
            pushWidget(new NetworkInfo(m_dbusNetwork));
    });

    connect(info_button, &ImageNameButton::stateChanged, [=](){
        if (info_button->getState()== ImageNameButton::Hover) {
            info_dynamicLabel->showLabel();
        }
    });
    connect(info_button, &ImageNameButton::stateChanged, [=](){
        if (info_button->getState() == ImageNameButton::Normal) {
            info_dynamicLabel->hideLabel();
        }
    });
}
