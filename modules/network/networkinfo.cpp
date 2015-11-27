#include <libdui/dheaderline.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlabel.h>
#include <libdui/dtextbutton.h>
#include <libdui/dthememanager.h>

#include "networkglobal.h"
#include "networkinfo.h"
#include "constants.h"
#include "networkmainwidget.h"

NetworkInfo::NetworkInfo(DBusNetwork *dbus, QWidget *parent) :
    DVBoxWidget(parent)
{
    init(dbus);
}

DHeaderLine *getLine(const QString &label, const QString &value)
{
    if(value.isEmpty())
        return nullptr;

    DHeaderLine *line = new DHeaderLine;
    DLabel *content = new DLabel(value);

    line->setTitle(label);
    line->setContent(content);
    line->setFixedSize(DCC::ModuleContentWidth, DUI::EXPAND_HEADER_HEIGHT);

    return line;
}

void NetworkInfo::init(DBusNetwork *dbus)
{
    m_mapConnectionTypeToName[ConnectionType::Wired] = tr("Ethernet");
    m_mapConnectionTypeToName[ConnectionType::Wireless] = tr("802.11 WiFi");
    m_mapConnectionTypeToName[ConnectionType::WirelessAdhoc] = tr("802.11 WiFi AdHoc");
    m_mapConnectionTypeToName[ConnectionType::WirelessHotspot] = tr("802.11 WiFi Hotspot");
    m_mapConnectionTypeToName[ConnectionType::Pppoe] = tr("PPPoE");
    m_mapConnectionTypeToName[ConnectionType::Mobile] = tr("Mobile");
    m_mapConnectionTypeToName[ConnectionType::MobileGsm] = tr("Mobile GSM");
    m_mapConnectionTypeToName[ConnectionType::MobileCdma] = tr("Mobile CDMA");
    m_mapConnectionTypeToName[ConnectionType::Vpn] = tr("VPN");
    m_mapConnectionTypeToName[ConnectionType::VpnL2tp] = tr("VPN L2TP");
    m_mapConnectionTypeToName[ConnectionType::VpnPptp] = tr("VPN PPTP");
    m_mapConnectionTypeToName[ConnectionType::VpnVpnc] = tr("VPN VPNC");
    m_mapConnectionTypeToName[ConnectionType::VpnOpenvpn] = tr("VPN OpenVPN");
    m_mapConnectionTypeToName[ConnectionType::VpnOpenconnect] = tr("VPN OpenConnect");

    DHeaderLine *header_line = new DHeaderLine;
    DSeparatorHorizontal *separator = new DSeparatorHorizontal;

    header_line->setFixedWidth(DCC::ModuleContentWidth);

    addWidget(header_line);
    addWidget(separator);

    ASYN_CALL(dbus->GetActiveConnectionInfo(), {
                  const QJsonDocument &json_doc = QJsonDocument::fromJson(dbus->GetActiveConnectionInfo().value().toUtf8());

                  if(json_doc.array().isEmpty()) {
                      header_line->setTitle(tr("Not Connected"));
                  } else {
                      header_line->setTitle(tr("Connected"));
                  }

                  auto getValueByLength = [](const QJsonArray &array, int length)->QJsonValue {
                      return array.count() > length ? array[length] : QJsonValue("");
                  };

                  for(const QJsonValue &value : json_doc.array()) {
                      const QJsonObject &json_obj = value.toObject();

                      DArrowLineExpand *expand = new DArrowLineExpand;
                      expand->setTitle(json_obj["IsPrimaryConnection"].toBool()
                      ? json_obj["ConnectionName"].toString() + tr("(default)")
                      : json_obj["ConnectionName"].toString());

                      DVBoxWidget *box_widget = new DVBoxWidget;

                      box_widget->setObjectName("expand_box_widget");
                      expand->setContent(box_widget);

                      QString line_value;

                      const QString &type = json_obj["ConnectionType"].toString();

                      if(type == ConnectionType::MobileGsm || type == ConnectionType::MobileCdma) {
                          line_value = m_mapConnectionTypeToName[type] + " " + json_obj["MobileNetworkType"].toString()
                                + " (" + json_obj["DeviceInterface"].toString() + ")";
                      } else {
                          line_value = m_mapConnectionTypeToName[type] + " (" + json_obj["DeviceInterface"].toString() + ")";
                      }

                      box_widget->addWidget(getLine(tr("Interface"), line_value));

                      const QJsonObject &json_ipv4 = json_obj["Ip4"].toObject();

                      box_widget->addWidget(getLine(tr("Security"), json_obj["Security"].toString()));
                      box_widget->addWidget(getLine(tr("Device Addr"), json_obj["HwAddress"].toString()));
                      box_widget->addWidget(getLine(tr("IP Address"), json_ipv4["Address"].toString()));
                      box_widget->addWidget(getLine(tr("Subnet Mask"), json_ipv4["Mask"].toString()));
                      box_widget->addWidget(getLine(tr("Gateway"), getValueByLength(json_ipv4["Gateways"].toArray(), 0).toString()));
                      box_widget->addWidget(getLine(tr("Primary DNS"), getValueByLength(json_ipv4["Dnses"].toArray(), 0).toString()));
                      box_widget->addWidget(getLine(tr("Secondary DNS"), getValueByLength(json_ipv4["Dnses"].toArray(), 1).toString()));
                      box_widget->addWidget(getLine(tr("Third DNS"), getValueByLength(json_ipv4["Dnses"].toArray(), 2).toString()));
                      box_widget->addWidget(getLine(tr("Device Speed"), json_obj["Speed"].toString()));

                      addWidget(expand);
                  }

                  DTextButton *button_cancel = new DTextButton(tr("Cancel"));
                  QHBoxLayout *layout_button = new QHBoxLayout;

                  connect(button_cancel, &DTextButton::clicked, this, [this] {
                      NetworkMainWidget *widget = DCCNetwork::parentNetworkMainWidget(this);

                      if(widget)
                          widget->popAllWidget();
                  });

                  layout_button->setMargin(0);
                  layout_button->setSpacing(0);
                  layout_button->addWidget(button_cancel, 0, Qt::AlignRight);
                  layout_button->addSpacing(15);

                  layout()->addSpacing(10);
                  layout()->addLayout(layout_button);
              }, this, dbus, header_line)
}
