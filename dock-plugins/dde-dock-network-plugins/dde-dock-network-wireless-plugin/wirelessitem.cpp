#include "wirelessitem.h"
#include <dde-dock/dockconstants.h>

WirelessItem::WirelessItem(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent)
    : QLabel(parent), m_uuid(uuid), m_dbusNetwork(dbusNetwork)
{
    setFixedSize(Dock::APPLET_EFFICIENT_ITEM_WIDTH, Dock::APPLET_EFFICIENT_ITEM_HEIGHT);

    m_applet = new WirelessApplet(uuid, dbusNetwork, this);
    connect(m_applet, &WirelessApplet::propertiesChanged, this, &WirelessItem::updateIcon);
    connect(m_applet, &WirelessApplet::sizeChanged, this, &WirelessItem::sizeChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &WirelessItem::updateIcon);
}

void WirelessItem::updateIcon()
{
    QString path = NetworkPlugin::getDeviceInfoById(m_uuid, m_dbusNetwork).path;
    bool connected = NetworkPlugin::getActiveConnectionDevices(m_dbusNetwork).indexOf(path) != -1;
    int strength = m_applet->maxStrength();
    QString iconPath = ":/images/images/ap-notconnect.png";

    if (strength >= 0 && connected) {
        strength = strength <= 5 ? 0 : (strength / 25 + int(strength % 25 > 0)) * 25;
        iconPath = ":/images/images/ap-signal-" + QString::number(strength) + ".png";
    }

    QPixmap icon(iconPath);
    setPixmap(icon.scaled(size()));
}

QString WirelessItem::uuid() const
{
    return m_uuid;
}

WirelessApplet *WirelessItem::applet() const
{
    return m_applet;
}
