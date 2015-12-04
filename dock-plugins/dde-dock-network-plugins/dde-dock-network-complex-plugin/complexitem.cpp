#include <QPainter>
#include <QDebug>
#include "complexitem.h"
#include "dde-dock/dockconstants.h"
#include "../network-data/networkdata.h"

const int SUB_ITEM_SIZE = 19;
const int ROUND_MARGIN = 2;

using namespace NetworkPlugin;

ComplexItem::ComplexItem(DBusNetwork *dbusNetwork, QWidget *parent)
    : QWidget(parent), m_dbusNetwork(dbusNetwork)
{
    setFixedSize(Dock::APPLET_FASHION_ITEM_WIDTH, Dock::APPLET_FASHION_ITEM_HEIGHT);

    connect(dbusNetwork, &DBusNetwork::DevicesChanged, this, &ComplexItem::manuallyUpdate);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &ComplexItem::manuallyUpdate);

    m_dbusBluetooth = new DBusBluetooth(this);
    connect(m_dbusBluetooth, &DBusBluetooth::StateChanged, this, &ComplexItem::manuallyUpdate);

    //for init
    manuallyUpdate();
}

void ComplexItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    drawBackground();
    drawVpn();
    drawMobile();
    drawBluetooth();
    drawWireless();

    m_manuallyUpdate = false;
}

void ComplexItem::drawBackground()
{
    if (m_manuallyUpdate)
        m_backgroundImg = wiredIsConnected(m_dbusNetwork)
                ? ":/images/images/network_online.png"
                : ":/images/images/network_offline.png";

    QPainter painter(this);
    painter.drawImage(geometry(), QImage(m_backgroundImg));
}

void ComplexItem::drawVpn()
{
    if (hasVpn(m_dbusNetwork)) {
        if (m_manuallyUpdate)
            m_vpnImg = vpnIsConnected(m_dbusNetwork)
                    ? ":/images/images/vpn_on.png"
                    : ":/images/images/vpn_off.png";

        QPainter painter(this);
        QRectF rect(ROUND_MARGIN, ROUND_MARGIN, SUB_ITEM_SIZE, SUB_ITEM_SIZE);
        painter.drawImage(rect, QImage(m_vpnImg));
    }
}

void ComplexItem::drawMobile()
{
    //TODO
}

void ComplexItem::drawBluetooth()
{
    if (m_dbusBluetooth->isValid() && m_dbusBluetooth->state() != BluetoothStateDisconnected) {
        if (m_manuallyUpdate)
            m_bluetoothImg = m_dbusBluetooth->state() == BluetoothStateConnected
                    ? ":/images/images/bluetooth_on.png"
                    : ":/images/images/bluetooth_off.png";

        QPainter painter(this);
        QRectF rect(ROUND_MARGIN,
                    height() - ROUND_MARGIN - SUB_ITEM_SIZE,
                    SUB_ITEM_SIZE,
                    SUB_ITEM_SIZE);
        painter.drawImage(rect, QImage(m_bluetoothImg));
    }
}

void ComplexItem::drawWireless()
{
    if (wirelessDevicesCount(m_dbusNetwork) > 0) {
        if (m_manuallyUpdate) {
            int maxStrength = wirelessApMaxStrength(m_dbusNetwork);
            int strengthLevel = 1;
            if (maxStrength > 75)
                strengthLevel = 5;
            else if (maxStrength > 50)
                strengthLevel = 4;
            else if (maxStrength > 25)
                strengthLevel = 3;
            else if (maxStrength > 0)
                strengthLevel = 2;

            m_wirelessImg = QString(":/images/images/wifi_%1.png").arg(strengthLevel);
        }
        QImage image(m_wirelessImg);
        QPainter painter(this);
        QRectF rect(width() - ROUND_MARGIN - SUB_ITEM_SIZE,
                    height() - ROUND_MARGIN - SUB_ITEM_SIZE,
                    SUB_ITEM_SIZE,
                    SUB_ITEM_SIZE);
        painter.drawImage(rect, image);
    }
}

void ComplexItem::manuallyUpdate()
{
    //for save resources
    m_manuallyUpdate = true;
    update();
}

