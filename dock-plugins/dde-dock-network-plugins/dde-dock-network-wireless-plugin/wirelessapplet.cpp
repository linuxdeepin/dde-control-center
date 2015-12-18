#include <QDBusPendingReply>
#include <QFile>

#include "wirelessapplet.h"
#include "../network-data/networkdata.h"

using namespace NetworkPlugin;

const int FRAME_WIDTH = 220;
const int TITLE_HEIGHT = 36;
const int LINE_HEIGHT = 24;
const int TITLE_LEFT_MARGIN = 20;
const int TITLE_RIGHT_MARGIN = 15;
const int CONTENT_MAX_HEIGHT = 300;

WirelessApplet::WirelessApplet(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent)
    : DVBoxWidget(parent), m_uuid(uuid), m_dbusNetwork(dbusNetwork)
{
    setFixedWidth(FRAME_WIDTH);

    initTitleLine();
    initApListContent();
    initStyleSheet();

    connect(m_dbusNetwork, &DBusNetwork::AccessPointAdded, this, &WirelessApplet::onAccessPointAdded);
    connect(m_dbusNetwork, &DBusNetwork::AccessPointRemoved, this, &WirelessApplet::onAccessPointRemoved);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WirelessApplet::onDevicesChanged);
    connect(m_dbusNetwork, &DBusNetwork::DeviceEnabled, this, &WirelessApplet::onDeviceEnabledChanged);
    connect(m_dbusNetwork, SIGNAL(DeviceEnabled(QString,bool)), this, SIGNAL(sizeChanged()));
}

WirelessApplet::~WirelessApplet()
{
    disconnect(m_dbusNetwork, &DBusNetwork::AccessPointAdded, this, &WirelessApplet::onAccessPointAdded);
    disconnect(m_dbusNetwork, &DBusNetwork::AccessPointRemoved, this, &WirelessApplet::onAccessPointRemoved);
    disconnect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WirelessApplet::onDevicesChanged);
    disconnect(m_dbusNetwork, &DBusNetwork::DeviceEnabled, this, &WirelessApplet::onDeviceEnabledChanged);
    disconnect(m_dbusNetwork, SIGNAL(DeviceEnabled(QString,bool)), this, SIGNAL(sizeChanged()));
}

int WirelessApplet::maxStrength()
{
    if (m_listWidget && m_listWidget->count() > 0) {
        WirelessAppletItem *item = qobject_cast<WirelessAppletItem *>(m_listWidget->widgetList().at(0));
        if (item) {
            return item->getApData().strength;
        }
    }
    return -1;
}

void WirelessApplet::initStyleSheet()
{
    QFile file(":/qss/qss/wireless.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else {
        qWarning() << "[WirelessPlugin] Open  style file errr!";
    }
}

void WirelessApplet::initTitleLine()
{
    QString title = tr("Wireless Network");
    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);
    if (wirelessDevices(m_dbusNetwork).count() > 1) {
        title = info.vendor;
    }
    m_titleLabel = new QLabel(title);
    m_titleLabel->setObjectName("DeviceTitle");
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_deviceSwitcher = new DSwitchButton();
    m_deviceSwitcher->setChecked(m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(info.path)));
    connect(m_deviceSwitcher, &DSwitchButton::checkedChanged, [=](bool checked){
        if (m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(info.path)) != checked) {
            m_dbusNetwork->EnableDevice(QDBusObjectPath(info.path), checked);
        }
    });

    QFrame *titleFrame = new QFrame(this);
    titleFrame->setFixedSize(FRAME_WIDTH, TITLE_HEIGHT);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleFrame);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addSpacing(TITLE_LEFT_MARGIN);
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_deviceSwitcher, 0, Qt::AlignRight);
    titleLayout->addSpacing(TITLE_RIGHT_MARGIN);

    addWidget(titleFrame);
    addWidget(new DSeparatorHorizontal());
}

void WirelessApplet::initApListContent()
{
    m_listWidget = new DListWidget;
    m_listWidget->setItemSize(FRAME_WIDTH, LINE_HEIGHT);
    m_listWidget->setEnableVerticalScroll(true);
    m_listWidget->setMaximumHeight(CONTENT_MAX_HEIGHT);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);

    ASYN_CALL(m_dbusNetwork->GetAccessPoints(QDBusObjectPath(info.path)), {
                  QJsonDocument jsonDoc = QJsonDocument::fromJson(args[0].toByteArray());
                  QVariantList mapList = jsonDoc.array().toVariantList();
                  qSort(mapList.begin(), mapList.end(), [](const QVariant &v1, const QVariant &v2)->bool{
                      return v1.toMap()["Strength"].toInt() > v2.toMap()["Strength"].toInt();
                  });

                  for (const QVariant &map : mapList) {
                      WirelessAppletItem::ApData data;
                      data.ssid = map.toMap().value("Ssid").toString();
                      data.apPath = map.toMap().value("Path").toString();
                      data.secured = map.toMap().value("Secured").toBool();
                      data.strength = map.toMap().value("Strength").toInt();
                      data.securedInEap = map.toMap().value("SecuredInEap").toBool();

                      addApToList(data);
                  }

                  //for init activeap
                  onDevicesChanged();
              }, this);

    m_listWidget->setVisible(m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(info.path)));

    addWidget(m_listWidget);
}

void WirelessApplet::onAccessPointAdded(const QString &devicePath, const QString &apInfo)
{
    if (devicePath != getDeviceInfoById(m_uuid, m_dbusNetwork).path)
        return;

    QVariantMap map = QJsonDocument::fromJson(apInfo.toUtf8()).object().toVariantMap();
    WirelessAppletItem::ApData data;
    data.ssid = map.value("Ssid").toString();
    data.apPath = map.value("Path").toString();
    data.secured = map.value("Secured").toBool();
    data.strength = map.value("Strength").toInt();
    data.securedInEap = map.value("SecuredInEap").toBool();

    addApToList(data);
}

void WirelessApplet::onAccessPointRemoved(const QString &devicePath, const QString &apInfo)
{
    if (devicePath != getDeviceInfoById(m_uuid, m_dbusNetwork).path)
        return;

    QVariantMap map = QJsonDocument::fromJson(apInfo.toUtf8()).object().toVariantMap();

    removeApFromList(map.value("Path").toString());
}

void WirelessApplet::onApStrengthChanged(int strength)
{
    //on wifi strength changed
    WirelessAppletItem *item = qobject_cast<WirelessAppletItem*>(sender());
    int index = m_listWidget->indexOf(item);

    for (int i = 0; i < m_listWidget->count(); ++i){
        WirelessAppletItem *tmpItem = qobject_cast<WirelessAppletItem*>(m_listWidget->getWidget(i));

        if(tmpItem && tmpItem->getApData().strength < strength) {
            if(index == i - 1)
                return;

            if(index >= 0)
                m_listWidget->removeWidget(index, false);

            if(index < i && index >= 0)
                m_listWidget->insertWidget(i - 1, item);
            else
                m_listWidget->insertWidget(i, item);

            return;
        }
    }
}

void WirelessApplet::onDevicesChanged()
{
    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);
    if (wirelessDevices(m_dbusNetwork).count() > 1) {
        m_titleLabel->setText(info.vendor);
    }
    else {
        m_titleLabel->setText(tr("Wireless Network"));
    }

    emit activeApChanged(info.activeAp);
}

void WirelessApplet::onDeviceEnabledChanged(const QString &path, bool enable)
{
    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);
    if (info.path == path) {
        m_listWidget->setVisible(enable);
        m_deviceSwitcher->setChecked(enable);
    }
}

void WirelessApplet::addApToList(const WirelessAppletItem::ApData &apData)
{
    //TODO it should fix by backend
    if (!removeOverlapApFromList(apData))
        return;

    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);
    WirelessAppletItem *item = new WirelessAppletItem(apData, info.path, m_dbusNetwork, this);
    item->onActiveApChanged();

    connect(item, &WirelessAppletItem::strengthChanged, this, &WirelessApplet::onApStrengthChanged);
    connect(this, &WirelessApplet::activeApChanged, item, &WirelessAppletItem::onActiveApChanged);

    m_listWidget->addWidget(item);
}

bool WirelessApplet::removeOverlapApFromList(const WirelessAppletItem::ApData &apData)
{
    DeviceInfo info = getDeviceInfoById(m_uuid, m_dbusNetwork);

    QList<QWidget *> list = m_listWidget->widgetList();
    for (QWidget *widget : list) {
        WirelessAppletItem *item = qobject_cast<WirelessAppletItem *>(widget);
        if (item && item->getApData().ssid == apData.ssid) {
            //the new one is active ap or stronger
            if (apData.apPath == info.activeAp || apData.strength > item->getApData().strength) {
                m_listWidget->removeWidget(list.indexOf(widget));
                item->deleteLater();

                return true;
            }
            else {
                return false;
            }
        }
    }

    return true;
}

void WirelessApplet::removeApFromList(const QString &apPath)
{
    QList<QWidget *> list = m_listWidget->widgetList();
    for (QWidget *widget : list) {
        WirelessAppletItem *item = qobject_cast<WirelessAppletItem *>(widget);
        if (item && item->getApPath() == apPath) {
            m_listWidget->removeWidget(list.indexOf(widget));
            item->deleteLater();
        }
    }
}
