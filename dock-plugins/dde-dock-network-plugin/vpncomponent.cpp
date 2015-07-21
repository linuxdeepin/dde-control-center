#include <QIcon>
#include <QLabel>
#include <QDBusConnection>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <dock/dockconstants.h>

#include "vpncomponent.h"
#include "vpnapplet.h"
#include "plugins.h"

VPNComponent::VPNComponent(QObject *parent) :
    QObject(parent),
    m_enabled(false)
{
    QIcon::setThemeName("Deepin");

    m_dbusNetwork = new com::deepin::daemon::Network("com.deepin.daemon.Network",
                                                     "/com/deepin/daemon/Network",
                                                     QDBusConnection::sessionBus(),
                                                     this);

    m_item = new QLabel;
    m_item->setFixedSize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);

    this->updateItem();

    connect(m_dbusNetwork, &Network::VpnEnabledChanged, this, &VPNComponent::updateItem);
    connect(m_dbusNetwork, &Network::ConnectionsChanged, this, &VPNComponent::updateItem);
    connect(m_dbusNetwork, &Network::ActiveConnectionsChanged, this, &VPNComponent::updateItem);
}

QString VPNComponent::getUUID()
{
    return "uuid_vpn";
}

QString VPNComponent::getTitle()
{
    return "VPN";
}

QWidget * VPNComponent::getItem()
{
    return m_dbusNetwork->vpnEnabled() ? m_item : NULL;
}

void VPNComponent::retainItem()
{
    m_item->setParent(NULL);
}

QWidget * VPNComponent::getApplet()
{
    return new VPNApplet(this);
}

QString VPNComponent::getMenuContent()
{
    return "";
}

void VPNComponent::invokeMenuItem(QString, bool)
{

}

// private methods
void VPNComponent::updateVPNs()
{
    m_enabled = m_dbusNetwork->vpnEnabled();

    m_vpns.clear();

    QString connectionsStr = m_dbusNetwork->connections();
    QJsonObject connections = QJsonDocument::fromJson(connectionsStr.toUtf8()).object();
    QJsonArray vpns = connections["vpn"].toArray();

    foreach (QJsonValue value, vpns) {
        QJsonObject vpnJson = value.toObject();

        VPN vpn;
        vpn.id = vpnJson["Id"].toString();
        vpn.uuid = vpnJson["Uuid"].toString();
        vpn.connected = isVPNConnected(vpn.uuid);

        m_vpns << vpn;
    } 

    emit vpnStatesChanged();
}

bool VPNComponent::isVPNConnected(QString uuid)
{
    QString activeConnectionsStr = m_dbusNetwork->activeConnections();
    QJsonObject activeConnections = QJsonDocument::fromJson(activeConnectionsStr.toUtf8()).object();

    foreach (QString key, activeConnections.keys()) {
        QJsonObject activeConnection = activeConnections[key].toObject();
        bool isVpnType = activeConnection["Vpn"].toBool();
        bool isConnected = activeConnection["State"].toInt() == 2;
        bool isUuidEqual = activeConnection["Uuid"].toString() == uuid;

        if (isUuidEqual) {
            if (isVpnType && isConnected) {
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}

bool VPNComponent::isAnyVPNConnected()
{
    foreach (VPN vpn, m_vpns) {
        if (vpn.connected) return true;
    }

    return false;
}

void VPNComponent::updateItem()
{
    updateVPNs();

    NetworkPlugin * np = qobject_cast<NetworkPlugin*>(parent());

    if (m_dbusNetwork->vpnEnabled()) {
        QString mark = isAnyVPNConnected() ? "active" : "disable";
        QString iconName = QString("network-vpn-%1-symbolic").arg(mark);

        m_item->setPixmap(QIcon::fromTheme(iconName).pixmap(m_item->size()));


        if (np && np->m_proxy) np->m_proxy->itemAddedEvent(getUUID());
    } else {

        // NOTE: reparent our poor m_item before dock deleting it.
        m_item->setParent(NULL);
        if (np && np->m_proxy) np->m_proxy->itemRemovedEvent(getUUID());
    }
}
