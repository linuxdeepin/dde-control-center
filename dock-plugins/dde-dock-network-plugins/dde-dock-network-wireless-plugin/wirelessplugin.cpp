
#include "wirelessplugin.h"

#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>
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
    if (m_mode != Dock::FashionMode) {
        onDevicesChanged();
    }
}

QString WirelessPlugin::getPluginName()
{
    return tr("Wireless Network");
}

QStringList WirelessPlugin::ids()
{
    QStringList list = m_mode == Dock::FashionMode ? QStringList() : wirelessDevices().keys();
    return list;
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

bool WirelessPlugin::configurable(const QString &id)
{
    return m_mode == Dock::FashionMode ? false : m_itemMap.keys().indexOf(id) != -1;
}

bool WirelessPlugin::enabled(const QString &id)
{
    QVariant value = m_settings->value(settingEnabledKey(id));
    return !value.isValid() ? true : value.toBool();    //default enabled
}

void WirelessPlugin::setEnabled(const QString &id, bool enabled)
{
    m_settings->setValue(settingEnabledKey(id), enabled);

    onEnabledChanged(id);
}

QWidget * WirelessPlugin::getApplet(QString id)
{
    if (m_itemMap.value(id))
        return m_itemMap.value(id)->applet();
    else
        return NULL;
}

QWidget * WirelessPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode)
        return NULL;
    else {
        if (enabled(id) && m_itemMap.value(id))
            return m_itemMap.value(id);
        else
            return NULL;
    }
}

void WirelessPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            foreach (QString id, m_itemMap.keys()) {
                removeItem(id);
                m_itemMap.take(id)->deleteLater();
            }
        }
        else {
            onDevicesChanged();
        }
    }
}

QString WirelessPlugin::getMenuContent(QString)
{
    return "";
}

void WirelessPlugin::invokeMenuItem(QString, QString, bool)
{

}

void WirelessPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-wireless-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        foreach (QString id, wirelessDevices().keys()) {
            setEnabled(id, true);
        }
    }
}

void WirelessPlugin::addNewItem(const QString &id, WirelessItem *item)
{
    m_itemMap.insert(id, item);

    m_proxy->itemAddedEvent(id);
    //add setting line to dock plugins setting frame
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
}

void WirelessPlugin::removeItem(const QString &id)
{
    m_proxy->itemRemovedEvent(id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
}

void WirelessPlugin::onEnabledChanged(const QString &id)
{
    if (!m_proxy)
        return;

    removeItem(id);

    if (enabled(id)) {
        WirelessItem *item = new WirelessItem(id, m_dbusNetwork);
        connect(item, &WirelessItem::sizeChanged, [=]{
            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, id);
        });
        addNewItem(id, item);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
}

int retryTimes = 10;
void WirelessPlugin::onDevicesChanged()
{
    if (!m_dbusNetwork->isValid()) {
        QTimer *retryTimer = new QTimer(this);
        retryTimer->setSingleShot(true);
        connect(retryTimer, &QTimer::timeout, this, &WirelessPlugin::onDevicesChanged);
        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
        retryTimer->start(1000);
        return;
    }
    qWarning() << "Network dbus data is ready!";
    retryTimes = 10;

    QStringList idList = wirelessDevices().keys();
    //remove old
    foreach (QString id, m_itemMap.keys()) {
        if (idList.indexOf(id) == -1)
            m_itemMap.take(id)->deleteLater();
    }

    //add new
    foreach (QString id, idList) {
        if (m_itemMap.keys().indexOf(id) == -1) {
            WirelessItem *item = new WirelessItem(id, m_dbusNetwork);
            connect(item, &WirelessItem::sizeChanged, [=]{
                m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, id);
            });
            addNewItem(id, item);
        }
        //update setting state
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeTitle, id);
    }
}

QMap<QString, QString> WirelessPlugin::wirelessDevices()
{
    QMap<QString, QString> tmpMap;
    if (m_dbusNetwork->isValid()) {
        QJsonArray pathArray = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWireless, m_dbusNetwork);
        foreach (QJsonValue pathValue, pathArray) {
            tmpMap.insert(pathValue.toObject().value("UniqueUuid").toString(),
                          pathValue.toObject().value("Vendor").toString());
        }
    }

    return tmpMap;
}

QString WirelessPlugin::settingEnabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_enabled").arg(id);
}
