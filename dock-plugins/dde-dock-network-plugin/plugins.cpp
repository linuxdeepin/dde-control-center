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

    m_items[bluetooth->getId()] = bluetooth;
    m_items[vpn->getId()] = vpn;
    m_items[wired->getId()] = wired;

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

    if (m_mode == Dock::FashionMode) {
        m_proxy->itemAddedEvent(m_composite->getId());
    } else {
        foreach (QString id, m_items.keys()) {
            m_proxy->itemAddedEvent(id);
        }
    }
}

QString NetworkPlugin::getPluginName()
{
    return "Network plugin";
}

QStringList NetworkPlugin::ids()
{
    if (m_mode == Dock::FashionMode) {
        return QStringList(m_composite->getId());
    } else {
        return m_items.keys();
    }
}

QString NetworkPlugin::getName(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getName();
    } else {
        return m_items.value(id)->getName();
    }
}

QString NetworkPlugin::getTitle(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getTitle();
    } else {
        return m_items.value(id)->getTitle();
    }
}

QString NetworkPlugin::getCommand(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getCommand();
    } else {
        return m_items.value(id)->getCommand();
    }
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
        if (id == m_composite->getId()) {
            m_composite->retainItem();
        } else {
            m_items[id]->retainItem();
        }

        m_proxy->itemRemovedEvent(id);
    }
}

QWidget * NetworkPlugin::getApplet(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getApplet();
    } else {
        return m_items.value(id)->getApplet();
    }
}

QWidget * NetworkPlugin::getItem(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getItem();
    } else {
        return m_items.value(id)->getItem();
    }
}

void NetworkPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;

    if (newMode == Dock::FashionMode && oldMode != Dock::FashionMode) {
        foreach (QString id, m_items.keys()) {
            m_items[id]->retainItem();
            m_proxy->itemRemovedEvent(id);
        }
        m_proxy->itemAddedEvent(m_composite->getId());
    } else if (oldMode == Dock::FashionMode && newMode != Dock::FashionMode) {
        m_composite->retainItem();
        m_proxy->itemRemovedEvent(m_composite->getId());
        foreach (QString id, m_items.keys()) {
            m_proxy->itemAddedEvent(id);
        }
    }
}

QString NetworkPlugin::getMenuContent(QString id)
{
    if (id == m_composite->getId()) {
        return m_composite->getMenuContent();
    } else {
        return m_items.value(id)->getMenuContent();
    }
}

void NetworkPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    if (id == m_composite->getId()) {
        return m_composite->invokeMenuItem(itemId, checked);
    } else {
        m_items.value(id)->invokeMenuItem(itemId, checked);
    }
}
