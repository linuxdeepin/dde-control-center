#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H

#include <QObject>
#include <QWidget>

#include "dbus/dbusnetwork.h"
#include "dbus/dbusbluetooth.h"

class ComplexItem : public QWidget
{
    Q_OBJECT
public:
    explicit ComplexItem(DBusNetwork *dbusNetwork, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void drawBackground();
    void drawVpn();
    void drawMobile();
    void drawBluetooth();
    void drawWireless();
    void manuallyUpdate();

private:
    DBusBluetooth *m_dbusBluetooth;
    DBusNetwork *m_dbusNetwork;
    QImage m_cacheImg;
    bool m_manuallyUpdate;
    QString m_backgroundImg;
    QString m_vpnImg;
    QString m_wirelessImg;
    QString m_bluetoothImg;
};

#endif // COMPLEXITEM_H
