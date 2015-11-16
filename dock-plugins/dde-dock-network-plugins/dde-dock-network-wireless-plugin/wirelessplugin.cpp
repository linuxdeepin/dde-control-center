
#include "wirelessplugin.h"

#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <QTimer>

#include <QDBusConnection>

WirelessPlugin::WirelessPlugin()
{
    QIcon::setThemeName("Deepin");

    m_dbusNetwork = new com::deepin::daemon::DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WirelessPlugin::onDevicesChanged);

    initSettings();
}

WirelessPlugin::~WirelessPlugin()
{

}

void WirelessPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();
    //for init
    if (m_mode != Dock::FashionMode)
        onDevicesChanged();
}

QString WirelessPlugin::getPluginName()
{
    return tr("Wireless Network");
}

QStringList WirelessPlugin::ids()
{
    return wirelessDevices().keys();
}

QString WirelessPlugin::getName(QString id)
{
    return wirelessDevices().count() > 1 ? wirelessDevices().value(id) : getPluginName();
}

QString WirelessPlugin::getTitle(QString id)
{
    return getName(id);
}

QString WirelessPlugin::getCommand(QString)
{
    return "dde-control-center network";
}

QPixmap WirelessPlugin::getIcon(QString)
{
    QPixmap icon(":/images/images/ap-signal-100.png");
    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool WirelessPlugin::canDisable(QString)
{
    return true;
}

bool WirelessPlugin::isDisabled(QString id)
{
    return m_settings->value(settingDisabledKey(id)).toBool();
}

void WirelessPlugin::setDisabled(QString id, bool disabled)
{
    m_settings->setValue(settingDisabledKey(id), disabled);

    onDisableChanged(id);
}

QWidget * WirelessPlugin::getApplet(QString id)
{
    return m_itemMap.value(id)->applet();
}

QWidget * WirelessPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode)
        return NULL;
    else
        return m_itemMap.value(id);
}

void WirelessPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            foreach (QString id, m_itemMap.keys()) {
                m_proxy->itemRemovedEvent(id);
                m_itemMap.value(id)->deleteLater();
                m_itemMap.remove(id);
            }
        }
        else if (oldMode == Dock::FashionMode){
            onDevicesChanged();
        }
    }
}

QString WirelessPlugin::getMenuContent(QString)
{
    //TODO
    return "";
}

void WirelessPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    //TODO
}

// private methods
void WirelessPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-wireless-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        foreach (QString id, wirelessDevices()) {
            m_settings->setValue(settingDisabledKey(id), false);
        }
    }
}

void WirelessPlugin::onDisableChanged(const QString &id)
{
    m_proxy->itemRemovedEvent(id);

    if (!isDisabled(id)) {
        WirelessItem *item = new WirelessItem(id, m_dbusNetwork);
        connect(item, &WirelessItem::sizeChanged, [=]{
            m_proxy->infoChangedEvent(DockPluginInterface::AppletSize, id);
        });
        m_itemMap.insert(id, item);

        m_proxy->itemAddedEvent(id);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::CanDisable, id);
}

void WirelessPlugin::onDevicesChanged()
{
    QJsonArray pathArray = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWireless, m_dbusNetwork);

    QStringList idList;
    foreach (QJsonValue value, pathArray) {
        idList << value.toObject().value("UniqueUuid").toString();
    }

    //remove old
    QStringList oldIdList;
    foreach (QString id, m_itemMap.keys()) {
        if (idList.indexOf(id) == -1)
            oldIdList << id;
    }
    foreach (QString id, oldIdList) {
        m_proxy->itemRemovedEvent(id);

        m_itemMap.value(id)->deleteLater();
        m_itemMap.remove(id);
    }

    //add new
    foreach (QString id, idList) {
        if (m_itemMap.keys().indexOf(id) == -1) {
            WirelessItem *item = new WirelessItem(id, m_dbusNetwork);
            connect(item, &WirelessItem::sizeChanged, [=]{
                m_proxy->infoChangedEvent(DockPluginInterface::AppletSize, id);
            });
            m_itemMap.insert(id, item);

            m_proxy->itemAddedEvent(id);
        }
    }
}

QMap<QString, QString> WirelessPlugin::wirelessDevices()
{
    QMap<QString, QString> tmpMap;
    QJsonArray pathArray = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWireless, m_dbusNetwork);
    foreach (QJsonValue pathValue, pathArray) {
        tmpMap.insert(pathValue.toObject().value("UniqueUuid").toString(),
                      pathValue.toObject().value("Vendor").toString());
    }

    return tmpMap;
}

QStringList WirelessPlugin::wiredDevicePaths()
{
    QStringList tmpList;
    QJsonArray pathArray = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWired, m_dbusNetwork);
    foreach (QJsonValue pathValue, pathArray) {
        tmpList << pathValue.toObject().value("Path").toString();
    }

    return tmpList;
}

QString WirelessPlugin::settingDisabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_disabled").arg(id);
}
