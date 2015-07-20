#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>

#include "plugins.h"

NetworkPlugin::NetworkPlugin() :
    m_proxy(NULL)
{
    BluetoothComponent * bluetooth = new BluetoothComponent(this);
    VPNComponent * vpn = new VPNComponent(this);

    m_items[bluetooth->getUUID()] = bluetooth;
    m_items[vpn->getUUID()] = vpn;
}

NetworkPlugin::~NetworkPlugin()
{

}

void NetworkPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    foreach (QString uuid, m_items.keys()) {
        m_proxy->itemAddedEvent(uuid);
    }
}

QString NetworkPlugin::name()
{
    return "Network plugin";
}

QStringList NetworkPlugin::uuids()
{
    return m_items.keys();
}

QString NetworkPlugin::getTitle(QString uuid)
{
    return m_items.value(uuid)->getTitle();
}

QWidget * NetworkPlugin::getApplet(QString uuid)
{
    return m_items.value(uuid)->getApplet();
}

QWidget * NetworkPlugin::getItem(QString uuid)
{
    return m_items.value(uuid)->getItem();
}

void NetworkPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{

}

QString NetworkPlugin::getMenuContent(QString uuid)
{
    return m_items.value(uuid)->getMenuContent();
}

void NetworkPlugin::invokeMenuItem(QString uuid, QString itemId, bool checked)
{
    m_items.value(uuid)->invokeMenuItem(itemId, checked);
}
