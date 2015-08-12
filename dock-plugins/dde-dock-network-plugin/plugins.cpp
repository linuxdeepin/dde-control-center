#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>

#include "plugins.h"

const static QString BluetoothComponentId = "id_network_bluetooth_plugin";
const static QString VpnComponentId = "id_network_vpn_plugin";
const static QString WiredComponentId = "id_network_wired_plugin";
const static QString CompositeComponentId = "id_network_composite_plugin";

NetworkPlugin::NetworkPlugin() :
    m_proxy(NULL)
{
    m_items[BluetoothComponentId] = new BluetoothComponent(BluetoothComponentId, this);
    m_items[VpnComponentId] = new VPNComponent(VpnComponentId, this);
    m_items[WiredComponentId] = new WiredComponent(WiredComponentId, this);
    m_items[CompositeComponentId] = new CompositeComponent(CompositeComponentId, this);

    initSettings();
}

NetworkPlugin::~NetworkPlugin()
{

}

void NetworkPlugin::initSettings()
{

}

void NetworkPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_mode = m_proxy->dockMode();

    foreach (QString id, ids()) {
        m_proxy->itemAddedEvent(id);
    }
}

QString NetworkPlugin::getPluginName()
{
    return "Network plugin";
}

QStringList NetworkPlugin::ids()
{
    if (m_mode == Dock::FashionMode) {
        return QStringList(CompositeComponentId);
    } else {
        QStringList keys = m_items.keys();
        keys.removeOne(CompositeComponentId);

        return keys;
    }
}

QString NetworkPlugin::getName(QString id)
{
    return m_items.value(id)->getName();
}

QString NetworkPlugin::getTitle(QString id)
{
    return m_items.value(id)->getTitle();
}

QString NetworkPlugin::getCommand(QString id)
{
    return m_items.value(id)->getCommand();
}

bool NetworkPlugin::canDisable(QString)
{
    return true;
}

bool NetworkPlugin::isDisabled(QString id)
{
    return false;
}

void NetworkPlugin::setDisabled(QString id, bool disabled)
{
    if (disabled) {
        m_items[id]->retainItem();

        m_proxy->itemRemovedEvent(id);
    }
}

QWidget * NetworkPlugin::getApplet(QString id)
{
    return m_items.value(id)->getApplet();
}

QWidget * NetworkPlugin::getItem(QString id)
{
    return m_items.value(id)->getItem();
}

void NetworkPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;

    if (newMode == Dock::FashionMode && oldMode != Dock::FashionMode) {
        QStringList keys = m_items.keys();
        keys.removeOne(CompositeComponentId);

        foreach (QString id, keys) {
            m_items[id]->retainItem();
            m_proxy->itemRemovedEvent(id);
        }

        m_proxy->itemAddedEvent(CompositeComponentId);
    } else if (oldMode == Dock::FashionMode && newMode != Dock::FashionMode) {
        PluginComponentInterface * compositeItem = m_items[CompositeComponentId];
        compositeItem->retainItem();
        m_proxy->itemRemovedEvent(CompositeComponentId);

        QStringList keys = m_items.keys();
        keys.removeOne(CompositeComponentId);
        foreach (QString id, keys) {
            m_proxy->itemAddedEvent(id);
        }
    }
}

QString NetworkPlugin::getMenuContent(QString id)
{
    return m_items.value(id)->getMenuContent();
}

void NetworkPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    m_items.value(id)->invokeMenuItem(itemId, checked);
}
