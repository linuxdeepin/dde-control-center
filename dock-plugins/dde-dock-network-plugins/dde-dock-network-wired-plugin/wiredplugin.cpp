
#include "wiredplugin.h"

#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <QDBusConnection>

const QString WIRED_PLUGIN_ID = "wired_plugin_id";
WiredPlugin::WiredPlugin()
{
    QIcon::setThemeName("Deepin");

    m_dbusNetwork = new com::deepin::daemon::DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WiredPlugin::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &WiredPlugin::onConnectionsChanged);

    initSettings();
}

WiredPlugin::~WiredPlugin()
{

}

void WiredPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();
    //for init
    if (m_mode != Dock::FashionMode) {
        m_wiredItem = nullptr;
        onConnectionsChanged();
    }
}

QString WiredPlugin::getPluginName()
{
    return tr("Wired network");
}

QStringList WiredPlugin::ids()
{
    if (m_mode != Dock::FashionMode && wirelessDevicesCount() == 0 && wiredDevicesCount() > 0) {
        return QStringList(WIRED_PLUGIN_ID);
    }
    else {
        return QStringList();
    }
}

QString WiredPlugin::getName(QString id)
{
    Q_UNUSED(id);

    return tr("Wired network");
}

QString WiredPlugin::getTitle(QString id)
{
    Q_UNUSED(id)

    return isWiredConnected() ? getWiredIp() : tr("Network disconnected");
}

QString WiredPlugin::getCommand(QString)
{
    return "dde-control-center network";
}

QPixmap WiredPlugin::getIcon(QString)
{
    QPixmap icon(":/images/images/wire_on.png");
    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool WiredPlugin::configurable(const QString &id)
{
    Q_UNUSED(id);

    return m_mode != Dock::FashionMode;
}

bool WiredPlugin::enabled(const QString &id)
{
    QVariant value = m_settings->value(settingEnabledKey(id));
    return !value.isValid() ? true : value.toBool();    //default enabled
}

void WiredPlugin::setEnabled(const QString &id, bool enabled)
{
    if (m_mode == Dock::FashionMode)
        return;

    m_settings->setValue(settingEnabledKey(id), enabled);

    onEnabledChanged(id);
}

QWidget * WiredPlugin::getApplet(QString id)
{
    Q_UNUSED(id)

    return NULL;
}

QWidget * WiredPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode)
        return NULL;
    else if (enabled(id)){
        if (m_wiredItem != nullptr)
            addNewItem(id);

        return m_wiredItem;
    }
    else
        return NULL;
}

void WiredPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            removeItem(WIRED_PLUGIN_ID);
        }
        else {
            onConnectionsChanged();
        }
    }
}

QString WiredPlugin::getMenuContent(QString)
{
    return "";
}

void WiredPlugin::invokeMenuItem(QString, QString, bool)
{

}

void WiredPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-wired-plugin", this);
}

void WiredPlugin::addNewItem(const QString &id)
{
    if (m_wiredItem != nullptr)
        return;

    m_wiredItem = new QLabel;
    m_wiredItem->setFixedSize(Dock::APPLET_EFFICIENT_ITEM_WIDTH, Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
    QString iconPath = isWiredConnected() ? ":/images/images/wire_on.png" : ":/images/images/network-error.png";
    m_wiredItem->setPixmap(QPixmap(iconPath).scaled(m_wiredItem->size()));
    m_proxy->itemAddedEvent(id);

    //add setting line to dock plugins setting frame
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    //update setting state
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeTitle, id);
}

void WiredPlugin::removeItem(const QString &id)
{
    if (m_wiredItem != nullptr) {
        m_proxy->itemRemovedEvent(id);
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
        m_wiredItem->deleteLater();
        m_wiredItem = nullptr;
        //remove setting line from dock plugins setting frame,should delete wireditem first
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    }
}

void WiredPlugin::onEnabledChanged(const QString &id)
{
    if (!m_proxy)
        return;

    removeItem(id);

    if (enabled(id)) {
        addNewItem(id);
    }
}

int retryTimes = 10;
void WiredPlugin::onConnectionsChanged()
{
    if (!m_dbusNetwork->isValid()) {
        QTimer *retryTimer = new QTimer;
        retryTimer->setSingleShot(true);
        connect(retryTimer, &QTimer::timeout, this, &WiredPlugin::onConnectionsChanged);
        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
        retryTimer->start(1000);
        return;
    }
    qWarning() << "DevicesChanged,Network dbus data is ready!";
    retryTimes = 10;

    removeItem(WIRED_PLUGIN_ID);

    if (wirelessDevicesCount() == 0 && wiredDevicesCount() > 0) {
        addNewItem(WIRED_PLUGIN_ID);
    }
}

QString WiredPlugin::getWiredIp()
{
    if (m_dbusNetwork->isValid()) {
        QList<NetworkPlugin::ActiveConnectionInfo> infoList = NetworkPlugin::getActiveConnectionsInfo(m_dbusNetwork);
        foreach (NetworkPlugin::ActiveConnectionInfo info, infoList) {
            if (info.connectionType == NetworkPlugin::ConnectionTypeWired)
                return info.ip4["Address"].toString();
        }

    }
    return QString();
}

bool WiredPlugin::isWiredConnected()
{
    if (!m_dbusNetwork->isValid())
        return false;

    QList<NetworkPlugin::ActiveConnectionInfo> infoList = NetworkPlugin::getActiveConnectionsInfo(m_dbusNetwork);
    foreach (NetworkPlugin::ActiveConnectionInfo info, infoList) {
        if (info.connectionType == NetworkPlugin::ConnectionTypeWired)
            return true;
    }

    return false;
}

int WiredPlugin::wirelessDevicesCount()
{
    if (m_dbusNetwork->isValid()) {
        QJsonArray array = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWireless, m_dbusNetwork);
        return array.size();
    }
    else {
        return 0;
    }
}

int WiredPlugin::wiredDevicesCount()
{
    if (m_dbusNetwork->isValid()) {
        QJsonArray array = NetworkPlugin::deviceArray(NetworkPlugin::ConnectionTypeWired, m_dbusNetwork);
        return array.size();
    }
    else {
        return 0;
    }
}

QString WiredPlugin::settingEnabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_enabled").arg(id);
}
