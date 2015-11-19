#include <QHBoxLayout>
#include "wirelessappletitem.h"
#include <dde-dock/dockconstants.h>

const int STRENGTH_STEP = 25;
const int ITEM_LEFT_MARGIN = 5;
const int ITEM_RIGHT_MARGIN = 15;

WirelessAppletItem::WirelessAppletItem(const ApData &data, const QString &devicePath, DBusNetwork *dbusNetwork, QWidget *parent)
    : QFrame(parent), m_dbusNetwork(dbusNetwork), m_devicePath(devicePath), m_apData(data)
{
    connect(dbusNetwork, &DBusNetwork::AccessPointPropertiesChanged, this, &WirelessAppletItem::onAccessPointPropertiesChanged);
    connect(dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &WirelessAppletItem::onActiveConnectionsChanged);

    QFrame *checkFrame = new QFrame;
    checkFrame->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);

    m_checkIcon = new DImageButton(checkFrame);
    m_checkIcon->setNormalPic(":/images/images/connected.png");
    m_checkIcon->setHoverPic(":/images/images/disconnect.png");
    m_checkIcon->setPressPic(":/images/images/disconnect.png");
    m_checkIcon->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
    m_checkIcon->setVisible(false);
    connect(m_checkIcon, &DImageButton::clicked, [=]{
        qWarning() << "Disconnect: " << getApPath();
        ASYN_CALL(m_dbusNetwork->DisconnectDevice(QDBusObjectPath(m_devicePath)), {Q_UNUSED(args)}, this)
    });

    m_loadingIcon = new DLoadingIndicator(checkFrame);
    m_loadingIcon->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
    m_loadingIcon->setImageSource(QPixmap(":/images/images/waiting.png"));
    m_loadingIcon->setLoading(true);
    m_loadingIcon->setVisible(false);

    m_title = new QPushButton(data.ssid);
    m_title->setObjectName("ApTitle");
    connect(m_title, &QPushButton::clicked, [=] {
        qWarning() << "Try to active ap: " << getApPath();
        ASYN_CALL(m_dbusNetwork->ActivateAccessPoint(getApUuidBySsid(m_apData.ssid, m_dbusNetwork),
                                                     QDBusObjectPath(getApPath()),
                                                     QDBusObjectPath(m_devicePath)), {Q_UNUSED(args)}, this)
    });

    m_strengthIcon = new QLabel();
    m_strengthIcon->setPixmap(getPixmapByStrength());
    m_strengthIcon->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(ITEM_LEFT_MARGIN, 0, ITEM_RIGHT_MARGIN, 0);
    layout->setSpacing(0);
    layout->addWidget(checkFrame);
    layout->addWidget(m_title);
    layout->addWidget(m_strengthIcon, 0, Qt::AlignRight | Qt::AlignVCenter);
}

QString WirelessAppletItem::getApPath() const
{
    return m_apData.apPath;
}

WirelessAppletItem::ApData WirelessAppletItem::getApData() const
{
    return m_apData;
}

void WirelessAppletItem::onActiveApChanged()
{
    updateConnectionState();
}

void WirelessAppletItem::onActiveConnectionsChanged()
{
    updateConnectionState();
}

void WirelessAppletItem::onAccessPointPropertiesChanged(const QString &devicePath, const QString &info)
{
    Q_UNUSED(info)
    QVariantMap map = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8()).object().toVariantMap();
    if (devicePath == m_devicePath && map.value("Path").toString() == getApPath()) {
        emit strengthChanged(map.value("Strength").toInt());
    }
}

QPixmap WirelessAppletItem::getPixmapByStrength()
{
    int strength = m_apData.strength;
    strength = strength <= 5 ? 0 : (strength / STRENGTH_STEP + int(strength % STRENGTH_STEP > 0)) * STRENGTH_STEP;
    QString iconPath = ":/images/images/ap-signal-" + QString::number(strength) + (m_apData.secured ? "-secure" : "")+ ".png";

    return QPixmap(iconPath);
}

void WirelessAppletItem::updateConnectionState()
{
    QString uuid = getApUuidBySsid(m_apData.ssid, m_dbusNetwork);
    ConnectionState state = getActiveConnectionsStateByUuid(uuid, m_dbusNetwork);
    switch (state) {
    case ActiveConnectionStateActivating:
    case ActiveConnectionStateDeactivating:
        m_loadingIcon->show();
        m_loadingIcon->setLoading(true);
        m_checkIcon->setVisible(false);
        break;
    case ActiveConnectionStateActivated:
        m_loadingIcon->setVisible(false);
        m_loadingIcon->setLoading(false);
        m_checkIcon->show();
        break;
    default:
        m_loadingIcon->setVisible(false);
        m_loadingIcon->setLoading(false);
        m_checkIcon->setVisible(false);
    }
}
