#include <QLabel>
#include <QFile>
#include <QDBusConnection>

#include "vpnplugin.h"
#include "vpnapplet.h"

using namespace NetworkPlugin;

const QString VPN_PLUGIN_ID = "vpn_plugin_id";
VpnPlugin::VpnPlugin()
{
    m_dbusNetwork = new com::deepin::daemon::DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::VpnEnabledChanged, this, &VpnPlugin::updateIcon);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &VpnPlugin::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &VpnPlugin::onConnectionsChanged);

    initSettings();
}

VpnPlugin::~VpnPlugin()
{
    qDebug() << "[VpnPlugin] VpnPlugin Destroyed!";
}

void VpnPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();
    m_vpnItem = nullptr;
    m_applet = nullptr;
    //for init
    if (m_mode != Dock::FashionMode && hasVpn(m_dbusNetwork)) {
        onConnectionsChanged();
    }
}

QString VpnPlugin::getPluginName()
{
    return tr("VPN");
}

QStringList VpnPlugin::ids()
{
    if (m_mode != Dock::FashionMode && hasVpn(m_dbusNetwork)) {
        return QStringList(VPN_PLUGIN_ID);
    }
    else {
        return QStringList();
    }
}

QString VpnPlugin::getName(QString id)
{
    Q_UNUSED(id);

    return tr("VPN");
}

QString VpnPlugin::getTitle(QString id)
{
    return getName(id);
}

QString VpnPlugin::getCommand(QString)
{
    return "dde-control-center network";
}

QPixmap VpnPlugin::getIcon(QString)
{
    QPixmap icon(":/images/images/vpn_on_16.png");
    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool VpnPlugin::configurable(const QString &id)
{
    Q_UNUSED(id);

    return (m_mode != Dock::FashionMode) && hasVpn(m_dbusNetwork);
}

bool VpnPlugin::enabled(const QString &id)
{
    return m_settings->value(settingEnabledKey(id), true).toBool();    //default enabled
}

void VpnPlugin::setEnabled(const QString &id, bool enabled)
{
    if (m_mode == Dock::FashionMode)
        return;

    m_settings->setValue(settingEnabledKey(id), enabled);

    onEnabledChanged(id);
}

QWidget * VpnPlugin::getApplet(QString id)
{
    Q_UNUSED(id)

    if (!m_dbusNetwork->isValid())
        return NULL;

    if (m_applet == nullptr) {
        m_applet = new VpnApplet(m_dbusNetwork);
        connect(m_applet, &VpnApplet::appletSizeChanged, [=]{
            m_proxy->infoChangedEvent(DockPluginInterface::AppletSize, VPN_PLUGIN_ID);
        });
    }

    return m_applet;
}

QWidget * VpnPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode) {
        return NULL;
    }
    else if (enabled(id)) {
        if (m_vpnItem == nullptr)
            addNewItem(id);

        return m_vpnItem;
    }
    else {
        return NULL;
    }
}

void VpnPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            removeItem(VPN_PLUGIN_ID);
        }
        else {
            onConnectionsChanged();
        }
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, VPN_PLUGIN_ID);
}

QString VpnPlugin::getMenuContent(QString)
{
    return "";
}

void VpnPlugin::invokeMenuItem(QString, QString, bool)
{

}

void VpnPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-vpn-plugin", this);
}

void VpnPlugin::updateIcon()
{
    if (m_vpnItem != nullptr) {
        QString iconPath = vpnIsConnected(m_dbusNetwork) ? ":/images/images/vpn_on_16.png" : ":/images/images/vpn_off_16.png";
        m_vpnItem->setPixmap(QPixmap(iconPath).scaled(m_vpnItem->size()));
    }
}

void VpnPlugin::addNewItem(const QString &id)
{
    if (m_vpnItem != nullptr)
        return;

    m_vpnItem = new QLabel;
    m_vpnItem->setFixedSize(Dock::APPLET_EFFICIENT_ITEM_WIDTH, Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
    updateIcon();
    m_proxy->itemAddedEvent(id);

    //add setting line to dock plugins setting frame
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    //update setting state
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeTitle, id);
}

void VpnPlugin::removeItem(const QString &id)
{
    if (m_vpnItem != nullptr) {
        m_vpnItem->setVisible(false);
        m_proxy->itemRemovedEvent(id);
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
        m_vpnItem->deleteLater();
        m_vpnItem = nullptr;
    }

    if (m_applet != nullptr) {
        m_applet->deleteLater();
        m_applet = nullptr;
    }
}

void VpnPlugin::onEnabledChanged(const QString &id)
{
    if (!m_proxy)
        return;

    removeItem(id);

    if (enabled(id)) {
        addNewItem(id);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
}

int retryTimes = 10;
void VpnPlugin::onConnectionsChanged()
{
    if (m_mode == Dock::FashionMode)
        return;

    if (!m_dbusNetwork->isValid() && retryTimes-- > 0) {
        QTimer *retryTimer = new QTimer;
        retryTimer->setSingleShot(true);
        connect(retryTimer, &QTimer::timeout, this, &VpnPlugin::onConnectionsChanged);
        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
        retryTimer->start(1000);
        qWarning() << "[VpnPlugin] Network dbus data is not ready!";
        return;
    }
    retryTimes = 10;


    if (hasVpn(m_dbusNetwork) && enabled(VPN_PLUGIN_ID)) {
        addNewItem(VPN_PLUGIN_ID);
    }
    else {
        removeItem(VPN_PLUGIN_ID);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, VPN_PLUGIN_ID);
    updateIcon();
}

QString VpnPlugin::settingEnabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_enabled").arg(id);
}
