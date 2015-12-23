#include <QLabel>
#include <QFile>
#include <QDBusConnection>

#include <libdui/dcheckbox.h>

#include "wirelessplugin.h"
#include "../network-data/networkdata.h"

using namespace NetworkPlugin;

WirelessPlugin::WirelessPlugin()
{
    m_dbusNetwork = new DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WirelessPlugin::onDevicesChanged);
    connect(m_dbusNetwork, &DBusNetwork::NeedSecrets, this, &WirelessPlugin::onNeedSecrets);

    initSettings();
}

WirelessPlugin::~WirelessPlugin()
{

}

void WirelessPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();
    updateUuids();
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
    QStringList list = m_mode == Dock::FashionMode ? QStringList() : m_uuids;
    return list;
}

QString WirelessPlugin::getName(QString id)
{
    QMap<QString, QString> tmpMap = wirelessDevices(m_dbusNetwork);
    if (tmpMap.count() > 1 && !tmpMap.value(id).isEmpty()) {
        return tmpMap.value(id);
    }
    else {
        return getPluginName();
    }
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
    return m_mode == Dock::FashionMode ? false : (m_uuids.indexOf(id) != -1);
}

bool WirelessPlugin::enabled(const QString &id)
{
    return m_settings->value(settingEnabledKey(id), true).toBool();    //default enabled
}

void WirelessPlugin::setEnabled(const QString &id, bool enabled)
{
    if (m_mode == Dock::FashionMode)
        return;

    m_settings->setValue(settingEnabledKey(id), enabled);

    onEnabledChanged(id);
}

QWidget * WirelessPlugin::getApplet(QString id)
{
    if (m_itemMap.value(id)) {
        return m_itemMap.value(id)->applet();
    }
    else {
        return NULL;
    }
}

QWidget * WirelessPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode) {
        return NULL;
    }
    else if (enabled(id)) {
        if (m_itemMap.value(id) == nullptr)
            addNewItem(id);

        return m_itemMap.value(id);
    }
    else {
        return NULL;
    }
}

void WirelessPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            for (QString id : m_uuids) {
                removeItem(id);
            }
        }
        else {
            onDevicesChanged();
        }
    }

    for (const QString id : m_itemMap.keys())
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
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
}

void WirelessPlugin::updateUuids()
{
    m_uuids = wirelessDevices(m_dbusNetwork).keys();
}

void WirelessPlugin::addNewItem(const QString &id)
{
    WirelessItem *item = new WirelessItem(id, m_dbusNetwork);
    connect(item, &WirelessItem::appletSizeChanged, [=]{
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, id);
    });

    m_itemMap.insert(id, item);
    m_proxy->itemAddedEvent(id);

    //add setting line to dock plugins setting frame
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    //update setting state
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeTitle, id);
}

void WirelessPlugin::removeItem(const QString &id)
{
    m_proxy->itemRemovedEvent(id);
    //remove setting line from dock plugins setting frame,should delete wirelessitem first
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);

    if (m_itemMap.keys().indexOf(id) != -1)
        m_itemMap.take(id)->deleteLater();
}

void WirelessPlugin::onEnabledChanged(const QString &id)
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
void WirelessPlugin::onDevicesChanged()
{
    if (m_mode == Dock::FashionMode)
        return;

    if (!m_dbusNetwork->isValid() && retryTimes-- > 0) {
        QTimer *retryTimer = new QTimer(this);
        retryTimer->setSingleShot(true);
        connect(retryTimer, &QTimer::timeout, this, &WirelessPlugin::onDevicesChanged);
        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
        retryTimer->start(1000);
        qWarning() << "[WirelessPlugin] Network dbus data is not ready!";
        return;
    }
    retryTimes = 10;
    updateUuids();

    //remove old
    for (QString id : m_itemMap.keys()) {
        if (m_uuids.indexOf(id) == -1)
            removeItem(id);
    }

    //add new
    for (QString id : m_uuids) {
        if (m_itemMap.keys().indexOf(id) == -1) {
            addNewItem(id);
        }
    }
}

void WirelessPlugin::onNeedSecrets(const QString &path, const QString &uuid, const QString &ssid, bool in3)
{
    QDBusInterface interface("com.deepin.dde.ControlCenter",
                             "/com/deepin/dde/ControlCenter/Network",
                             "com.deepin.dde.ControlCenter.Network");

    if(interface.isValid() && interface.call("active").arguments().first().toBool()) {
        qDebug() << "WirelessApplet:" << "dde-control-center network module is active.";

        return;
    }

    Q_UNUSED(in3)

    m_targetConnectPath = path;
    m_tragetConnectUuid = uuid;

    if(!m_passworkInputDialog) {
        m_passworkInputDialog = new DInputDialog;

        DCheckBox *check_box = new DCheckBox;

        check_box->setText(tr("Auto-connect"));

        QIcon::setThemeName("deepin");

        m_passworkInputDialog->setTextEchoMode(DLineEdit::Password);
        m_passworkInputDialog->setIcon(QIcon::fromTheme("notification-network-wireless-full"));
        m_passworkInputDialog->addSpacing(10);
        m_passworkInputDialog->addContent(check_box, Qt::AlignLeft);
        m_passworkInputDialog->setOkButtonText(tr("Connect"));

        connect(m_passworkInputDialog.data(), &DInputDialog::textValueChanged,
                this, [this]{m_passworkInputDialog->setTextAlert(false);});
        connect(m_passworkInputDialog.data(), &DInputDialog::okButtonClicked,
                this, [this, check_box] {
            if(!m_passworkInputDialog->textValue().isEmpty()) {
                m_dbusNetwork->FeedSecret(m_targetConnectPath, m_tragetConnectUuid, m_passworkInputDialog->textValue(),
                                          check_box->checkState() != Qt::Unchecked);
            } else {
                m_passworkInputDialog->setTextAlert(true);
            }
        });
        connect(m_passworkInputDialog.data(), &DInputDialog::cancelButtonClicked,
                m_passworkInputDialog, &DInputDialog::close);
        connect(m_passworkInputDialog.data(), &DInputDialog::closed, [this] {
            m_passworkInputDialog->deleteLater();
            m_dbusNetwork->CancelSecret(m_targetConnectPath, m_tragetConnectUuid);
        });

        m_passworkInputDialog->open();
    }


    m_passworkInputDialog->setTitle(tr("Please enter the password of <font color=\"#faca57\">%1</font>").arg(ssid));
    m_passworkInputDialog->raise();
}

QString WirelessPlugin::settingEnabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_enabled").arg(id);
}
