#ifndef WIRELESSITEM_H
#define WIRELESSITEM_H

#include <QLabel>
#include "wirelessapplet.h"
#include "dbus/dbusnetwork.h"
#include "../network-data/networkdata.h"

class WirelessItem : public QLabel
{
    Q_OBJECT
public:
    explicit WirelessItem(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent = 0);
    WirelessApplet *applet() const;

signals:
    void appletSizeChanged();

private:
    void updateIcon();

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    WirelessApplet *m_applet;
};

#endif // WIRELESSITEM_H
