#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>

#include "plugins.h"

NetworkPlugin::NetworkPlugin() :
    m_proxy(NULL)
{
    m_composite = new CompositeComponent(this);

    BluetoothComponent * bluetooth = new BluetoothComponent(this);
    VPNComponent * vpn = new VPNComponent(this);
    WiredComponent * wired = new WiredComponent(this);

    m_items[bluetooth->getUUID()] = bluetooth;
    m_items[vpn->getUUID()] = vpn;
    m_items[wired->getUUID()] = wired;
}

NetworkPlugin::~NetworkPlugin()
{

}

void NetworkPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_mode = m_proxy->dockMode();

    if (m_mode == Dock::FashionMode) {
        m_proxy->itemAddedEvent(m_composite->getUUID());
    } else {
        foreach (QString uuid, m_items.keys()) {
            m_proxy->itemAddedEvent(uuid);
        }
    }
}

QString NetworkPlugin::name()
{
    return "Network plugin";
}

QStringList NetworkPlugin::uuids()
{
    if (m_mode == Dock::FashionMode) {
        return QStringList(m_composite->getUUID());
    } else {
        return m_items.keys();
    }
}

QString NetworkPlugin::getTitle(QString uuid)
{
    if (m_mode == Dock::FashionMode) {
        return m_composite->getTitle();
    } else {
        return m_items.value(uuid)->getTitle();
    }
}

QWidget * NetworkPlugin::getApplet(QString uuid)
{
    if (m_mode == Dock::FashionMode) {
        return m_composite->getApplet();
    } else {
        return m_items.value(uuid)->getApplet();
    }
}

QWidget * NetworkPlugin::getItem(QString uuid)
{
    if (m_mode == Dock::FashionMode) {
        return m_composite->getItem();
    } else {
        return m_items.value(uuid)->getItem();
    }
}

void NetworkPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;

    if (newMode == Dock::FashionMode && oldMode != Dock::FashionMode) {
        foreach (QString uuid, m_items.keys()) {
            m_items[uuid]->retainItem();
            m_proxy->itemRemovedEvent(uuid);
        }
        m_proxy->itemAddedEvent(m_composite->getUUID());
    } else if (oldMode == Dock::FashionMode && newMode != Dock::FashionMode) {
        m_composite->retainItem();
        m_proxy->itemRemovedEvent(m_composite->getUUID());
        foreach (QString uuid, m_items.keys()) {
            m_proxy->itemAddedEvent(uuid);
        }
    }
}

QString NetworkPlugin::getMenuContent(QString uuid)
{
    return m_items.value(uuid)->getMenuContent();
}

void NetworkPlugin::invokeMenuItem(QString uuid, QString itemId, bool checked)
{
    m_items.value(uuid)->invokeMenuItem(itemId, checked);
}
