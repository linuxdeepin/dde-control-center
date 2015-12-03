#include <QDBusPendingReply>
#include <QFile>
#include "vpnapplet.h"
#include "../network-data/networkdata.h"

using namespace NetworkPlugin;

const int FRAME_WIDTH = 220;
const int TITLE_HEIGHT = 36;
const int LINE_HEIGHT = 24;
const int TITLE_LEFT_MARGIN = 20;
const int TITLE_RIGHT_MARGIN = 15;
const int CONTENT_MAX_HEIGHT = 300;

VpnApplet::VpnApplet(DBusNetwork *dbusNetwork, QWidget *parent)
    : DVBoxWidget(parent), m_dbusNetwork(dbusNetwork)
{
    setFixedWidth(FRAME_WIDTH);

    initTitleLine();
    initVpnListContent();
    initStyleSheet();

    connect(m_dbusNetwork, &DBusNetwork::VpnEnabledChanged, this, &VpnApplet::onVpnEnableChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &VpnApplet::onConnectionsChanged);
}

VpnApplet::~VpnApplet()
{
    disconnect(m_dbusNetwork, &DBusNetwork::VpnEnabledChanged, this, &VpnApplet::sizeChanged);
}

void VpnApplet::initStyleSheet()
{
    QFile file(":/qss/qss/vpn.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else {
        qWarning() << "[VpnPlugin] Open  style file errr!";
    }
}

void VpnApplet::initTitleLine()
{
    m_titleLabel = new QLabel(tr("VPN"));
    m_titleLabel->setObjectName("VpnTitle");
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_vpnSwitcher = new DSwitchButton();
    m_vpnSwitcher->setChecked(m_dbusNetwork->vpnEnabled());
    connect(m_vpnSwitcher, &DSwitchButton::checkedChanged, [=](bool checked){
        if (m_dbusNetwork->vpnEnabled() != checked) {
            m_dbusNetwork->setVpnEnabled(checked);
        }
    });

    QFrame *titleFrame = new QFrame(this);
    titleFrame->setFixedSize(FRAME_WIDTH, TITLE_HEIGHT);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleFrame);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->addSpacing(TITLE_LEFT_MARGIN);
    titleLayout->addWidget(m_titleLabel, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_vpnSwitcher, 0, Qt::AlignRight);
    titleLayout->addSpacing(TITLE_RIGHT_MARGIN);

    addWidget(titleFrame);
    addWidget(new DSeparatorHorizontal());
}

void VpnApplet::initVpnListContent()
{
    m_listWidget = new DListWidget;
    m_listWidget->setItemSize(FRAME_WIDTH, LINE_HEIGHT);
    m_listWidget->setEnableVerticalScroll(true);
    m_listWidget->setMaximumHeight(CONTENT_MAX_HEIGHT);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    addWidget(m_listWidget);

    onVpnEnableChanged();
    onConnectionsChanged();
}

void VpnApplet::onConnectionsChanged()
{
    QJsonArray vpnArray = connectiosArray(ConnectionTypeVpn, m_dbusNetwork);
    QList<VpnAppletItem::VpnData> dataList;
    QStringList tmpUuidList;
    for (QJsonValue value : vpnArray) {
        QJsonObject obj = value.toObject();
        if (!obj.isEmpty()) {
            VpnAppletItem::VpnData data;
            data.path = obj["Path"].toString();
            data.uuid = obj["Uuid"].toString();
            data.id = obj["Id"].toString();
            dataList << data;
            tmpUuidList << data.uuid;
        }
    }

    //remove old
    for (QString uuid : m_uuidList) {
        if (tmpUuidList.indexOf(uuid) == -1) {
            removeItemFromList(uuid);
        }
    }

    //add new
    for (VpnAppletItem::VpnData data : dataList) {
        if (m_uuidList.indexOf(data.uuid) == -1) {
            addItemToList(data);
        }
    }

    emit sizeChanged();
}

void VpnApplet::onVpnEnableChanged()
{
    emit sizeChanged();
    m_vpnSwitcher->setChecked(m_dbusNetwork->vpnEnabled());
    m_listWidget->setVisible(m_dbusNetwork->vpnEnabled());
}

void VpnApplet::addItemToList(const VpnAppletItem::VpnData &vpnData)
{
    VpnAppletItem *item = new VpnAppletItem(vpnData, m_dbusNetwork, this);

    m_uuidList << vpnData.uuid;
    m_listWidget->addWidget(item);
}

void VpnApplet::removeItemFromList(const QString &uuid)
{
    QList<QWidget *> list = m_listWidget->widgetList();
    for (QWidget *widget : list) {
        VpnAppletItem *item = qobject_cast<VpnAppletItem *>(widget);
        if (item && item->getItemUuid() == uuid) {
            m_listWidget->removeWidget(list.indexOf(widget));
            item->deleteLater();

            m_uuidList.removeAll(uuid);
        }
    }
}

