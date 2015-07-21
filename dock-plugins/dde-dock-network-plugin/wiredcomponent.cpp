#include <QIcon>
#include <QLabel>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "wiredcomponent.h"
#include "plugins.h"

WiredComponent::WiredComponent(QObject *parent) :
    QObject(parent)
{
    QIcon::setThemeName("Deepin");

    m_dbusNetwork = new com::deepin::daemon::Network("com.deepin.daemon.Network",
                                                     "/com/deepin/daemon/Network",
                                                     QDBusConnection::sessionBus(),
                                                     this);

    m_item = new QLabel;
    m_item->setFixedSize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);

    this->updateItem();

    connect(m_dbusNetwork, &Network::ConnectionsChanged, this, &WiredComponent::updateItem);
    connect(m_dbusNetwork, &Network::ActiveConnectionsChanged, this, &WiredComponent::updateItem);
}

QString WiredComponent::getUUID()
{
    return "uuid_wired";
}

QString WiredComponent::getTitle()
{
    return getWiredIP();
}

QWidget * WiredComponent::getItem()
{
    if (shouldShowItem()) {
        return m_item;
    }

    return NULL;
}

QWidget * WiredComponent::getApplet()
{
    return NULL;
}

QString WiredComponent::getMenuContent()
{
    return "";
}

void WiredComponent::invokeMenuItem(QString, bool)
{

}


// private methods
QString WiredComponent::getWiredIP()
{
    QDBusPendingReply<QString> reply = m_dbusNetwork->GetActiveConnectionInfo();
    reply.waitForFinished();

    if (reply.isValid()) {
        QJsonArray connections = QJsonDocument::fromJson(reply.value().toUtf8()).array();

        foreach (QJsonValue value, connections) {
            QJsonObject connection = value.toObject();
            if (connection["ConnectionType"].toString() == "wired") {
                return connection["Ip4"].toObject()["Address"].toString();
            }
        }
    } else {
        return "";
    }
}

bool WiredComponent::hasWiredDevices()
{
    QJsonObject devices = QJsonDocument::fromJson(m_dbusNetwork->devices().toUtf8()).object();

    if (devices["wired"].toArray().toVariantList().length() > 0) {
        return true;
    } else {
        return false;
    }
}

bool WiredComponent::hasWirelessDevices()
{
    QJsonObject devices = QJsonDocument::fromJson(m_dbusNetwork->devices().toUtf8()).object();

    if (devices["wireless"].toArray().toVariantList().length() > 0) {
        return true;
    } else {
        return false;
    }
}

bool WiredComponent::isAnyWiredActive()
{
    QJsonObject devices = QJsonDocument::fromJson(m_dbusNetwork->devices().toUtf8()).object();

    QJsonArray wiredDevices = devices["wired"].toArray();

    foreach (QJsonValue value, wiredDevices) {
        QJsonObject wiredDevice = value.toObject();

        if (wiredDevice["State"].toInt() == 100) {
            return true;
        }
    }

    return false;
}

bool WiredComponent::isAnyWirelessActive()
{
    QJsonObject devices = QJsonDocument::fromJson(m_dbusNetwork->devices().toUtf8()).object();

    QJsonArray wirelessDevices = devices["wireless"].toArray();

    foreach (QJsonValue value, wirelessDevices) {
        QJsonObject wirelessDevice = value.toObject();

        if (wirelessDevice["ActiveAp"].toString() != "/"
                && wirelessDevice["State"].toInt() == 100) {
            return true;
        }
    }

    return false;
}

bool WiredComponent::shouldShowItem()
{
    return (hasWiredDevices() && !hasWirelessDevices())
            || (isAnyWiredActive() && !isAnyWirelessActive());
}

void WiredComponent::updateItem()
{
    NetworkPlugin * np = qobject_cast<NetworkPlugin*>(parent());

    if (shouldShowItem()) {
        if (getWiredIP() != "") {
            m_item->setPixmap(QIcon::fromTheme("network-wired-symbolic").pixmap(m_item->size()));
        } else {
            m_item->setPixmap(QIcon::fromTheme("network-offline-symbolic").pixmap(m_item->size()));
        }

        if (np && np->m_proxy) np->m_proxy->itemAddedEvent(getUUID());
    } else {
        if (np && np->m_proxy) np->m_proxy->itemRemovedEvent(getUUID());
    }
}
