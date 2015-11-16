#include "wirelessappletitem.h"
#include <QHBoxLayout>

WirelessAppletItem::WirelessAppletItem(const ApData &data, const QString &devicePath, DBusNetwork *dbusNetwork, QWidget *parent)
    : QFrame(parent), m_apData(data), m_devicePath(devicePath), m_dbusNetwork(dbusNetwork)
{
    connect(dbusNetwork, &DBusNetwork::AccessPointPropertiesChanged, this, &WirelessAppletItem::onAccessPointPropertiesChanged);
    connect(dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &WirelessAppletItem::onActiveConnectionsChanged);

    QFrame *checkFrame = new QFrame;
    checkFrame->setFixedSize(16, 16);

    m_checkIcon = new DImageButton(checkFrame);
    m_checkIcon->setNormalPic(":/images/images/connected.png");
    m_checkIcon->setHoverPic(":/images/images/disconnect.png");
    m_checkIcon->setPressPic(":/images/images/disconnect.png");
    m_checkIcon->setFixedSize(16, 16);
    m_checkIcon->setVisible(false);
    connect(m_checkIcon, &DImageButton::clicked, [=]{
        qWarning() << "Disconnect: " << getApPath();
        ASYN_CALL(m_dbusNetwork->DisconnectDevice(QDBusObjectPath(m_devicePath)), {}, this)
    });

    m_loadingIcon = new DLoadingIndicator(checkFrame);
    m_loadingIcon->setFixedSize(16, 16);
    m_loadingIcon->setImageSource(QPixmap(":/images/images/waiting.png"));
    m_loadingIcon->setLoading(true);
    m_loadingIcon->setVisible(false);

    m_title = new QPushButton(data.ssid);
    m_title->setObjectName("ApTitle");
    connect(m_title, &QPushButton::clicked, [=] {
        qWarning() << "Try to active ap: " << getApPath();
        ASYN_CALL(m_dbusNetwork->ActivateAccessPoint(getApUuidBySsid(m_apData.ssid, m_dbusNetwork),
                                                     QDBusObjectPath(getApPath()),
                                                     QDBusObjectPath(m_devicePath)), {}, this)
    });

    m_strengthIcon = new QLabel();
    m_strengthIcon->setPixmap(getPixmapByStrength());
    m_strengthIcon->setFixedSize(16, 16);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 10, 0);
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

void WirelessAppletItem::onActiveApChanged(const QString &ap)
{
    updateConnectionState();
}

void WirelessAppletItem::onActiveConnectionsChanged()
{
    updateConnectionState();
}

void WirelessAppletItem::onAccessPointPropertiesChanged(const QString &devicePath, const QString &info)
{
    QVariantMap map = QJsonDocument::fromJson(m_dbusNetwork->activeConnections().toUtf8()).object().toVariantMap();
    if (devicePath == m_devicePath && map.value("Path").toString() == getApPath()) {
        emit strengthChanged(map.value("Strength").toInt());
    }
}

QPixmap WirelessAppletItem::getPixmapByStrength()
{
    int strength = m_apData.strength;
    strength = strength <= 5 ? 0 : (strength / 25 + int(strength % 25 > 0)) * 25;
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
