#include <QUuid>
#include <QLabel>
#include <QDebug>
#include <QIcon>
#include <QFile>

#include "plugins.h"

const static QString BluetoothComponentId = "id_network_bluetooth_plugin";
const static QString VpnComponentId = "id_network_vpn_plugin";
const static QString WiredComponentId = "id_network_wired_plugin";
const static QString CompositeComponentId = "id_network_composite_plugin";

static inline QString capitalize(QString str)
{
    return str.left(1).toUpper() + str.mid(1).toLower();
}

static QString idToEntry(const QString id)
{
    QString result;

    QString inner = id;
    inner.replace("id_network", "");

    QStringList tokens = inner.split("_");
    foreach (QString token, tokens) {
        result.append(capitalize(token));
    }

    return result;
}

static inline QString pair(QString entry, QString key)
{
    return entry + "/" + key;
}

NetworkPlugin::NetworkPlugin() :
    m_proxy(NULL)
{
    m_items[BluetoothComponentId] = new BluetoothComponent(BluetoothComponentId, this);
    m_items[VpnComponentId] = new VPNComponent(VpnComponentId, this);
    m_items[WiredComponentId] = new WiredComponent(WiredComponentId, this);
    m_items[CompositeComponentId] = new CompositeComponent(CompositeComponentId, this);

    this->initSettings();
}

NetworkPlugin::~NetworkPlugin()
{

}

void NetworkPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_mode = m_proxy->dockMode();

    onDisableChanged();
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

QPixmap NetworkPlugin::getIcon(QString id)
{
    return isDisabled(id) ? QPixmap("") : m_items.value(id)->getIcon();
}

bool NetworkPlugin::canDisable(QString)
{
    return true;
}

bool NetworkPlugin::isDisabled(QString id)
{
    return m_settings->value(settingDisabledKey(id)).toBool();
}

void NetworkPlugin::setDisabled(QString id, bool disabled)
{
    if (m_items.keys().indexOf(id) == -1)
        return;

    m_settings->setValue(settingDisabledKey(id), disabled);

    onDisableChanged();
}

QWidget * NetworkPlugin::getApplet(QString id)
{
    return m_items.value(id)->getApplet();
}

QWidget * NetworkPlugin::getItem(QString id)
{
    return isDisabled(id) ? NULL : m_items.value(id)->getItem();
}

void NetworkPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (newMode == oldMode)
        return;

    m_mode = newMode;

    onDisableChanged();
}

QString NetworkPlugin::getMenuContent(QString id)
{
    return m_items.value(id)->getMenuContent();
}

void NetworkPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    m_items.value(id)->invokeMenuItem(itemId, checked);
}

// private methods
void NetworkPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-plugin");

    if (!QFile::exists(m_settings->fileName())) {
        foreach (QString id, m_items.keys()) {
            m_settings->setValue(settingDisabledKey(id), false);
        }
    }
}

void NetworkPlugin::onDisableChanged()
{
    //remove add plugin-item and reload by onDisableChanged()
    foreach (QString uuid, m_items.keys()) {
        m_items[uuid]->retainItem();
        m_proxy->itemRemovedEvent(uuid);
    }

    foreach (QString id, ids()) {
        if (isDisabled(id)) {
            m_items[id]->retainItem();
            m_proxy->itemRemovedEvent(id);
        }
        else
            m_proxy->itemAddedEvent(id);
    }
}

QString NetworkPlugin::settingDisabledKey(const QString &uuid)
{
    return QString("%1/%2-disabled").arg(m_mode).arg(uuid);
}
