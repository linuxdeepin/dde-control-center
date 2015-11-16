#include <QDebug>
#include <QVBoxLayout>

#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dlistwidget.h>
#include <libdui/darrowlineexpand.h>

#include "moduleheader.h"
#include "constants.h"
#include "normallabel.h"
#include "imagenamebutton.h"
#include "genericlistitem.h"
#include "generaladdbutton.h"

#include "wirednetworklistitem.h"
#include "wirelessnetworklistitem.h"
#include "modemnetworklistitem.h"
#include "vpnconnectswidget.h"
#include "networkmainwidget.h"
#include "addconnectpage.h"
#include "systemproxywidget.h"

namespace DCCNetwork {
    NetworkMainWidget *parentNetworkMainWidget(const QObject *obj)
    {
        QObject *tmp_obj = obj->parent();

        while(tmp_obj) {
            NetworkMainWidget *widget = qobject_cast<NetworkMainWidget*>(tmp_obj);
            if(widget)
                return widget;
            tmp_obj = tmp_obj->parent();
        }

        return NULL;
    }
}

NetworkMainWidget::NetworkMainWidget(QWidget *parent) :
    ScrollFrame(parent)
{
    m_dbusNetwork = new DBusNetwork(this);

    initUI();
    updateUI();

    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &NetworkMainWidget::updateUI);
}

DBusNetwork *NetworkMainWidget::dbusNetwork() const
{
    return m_dbusNetwork;
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
    ModuleHeader *header = new ModuleHeader(tr("Network"), false);
    QWidget *header_right_widget = new QWidget;
    QHBoxLayout *header_right_widget_layout = new QHBoxLayout(header_right_widget);
    GeneralAddButton *add_button = new GeneralAddButton;
    //ImageNameButton *info_button = new ImageNameButton("info");

    header->setFixedSize(DCC::ModuleContentWidth, DUI::CONTENT_HEADER_HEIGHT);
    header_right_widget_layout->setMargin(0);
    header_right_widget_layout->setSpacing(5);
    header_right_widget_layout->addWidget(add_button);
    //header_right_widget_layout->addWidget(info_button);
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
        pushWidget(add_connect_page);
    });
}
