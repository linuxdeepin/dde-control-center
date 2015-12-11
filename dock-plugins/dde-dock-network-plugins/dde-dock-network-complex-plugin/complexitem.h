#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H

#include <QObject>
#include <QWidget>

#include <libdui/dinputdialog.h>

#include "dbus/dbusnetwork.h"
#include "dbus/dbusbluetooth.h"

DUI_USE_NAMESPACE

class ComplexItem : public QWidget
{
    Q_OBJECT
public:
    explicit ComplexItem(DBusNetwork *dbusNetwork, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void onNeedSecrets(const QString &in0, const QString &in1, const QString &in2, bool in3);

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

    DInputDialog *m_passworkInputDialog = nullptr;
    QString m_targetDevicePath;
    QString m_tragetConnectUuid;
};

#endif // COMPLEXITEM_H
