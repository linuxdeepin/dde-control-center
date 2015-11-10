#ifndef NETWORKMAINWIDGET_H
#define NETWORKMAINWIDGET_H

#include "scrollframe.h"
#include "dbus/dbusnetwork.h"

class QVBoxLayout;
class WiredNetworkListItem;
class WirelessNetworkListItem;
class VPNConnectsWidget;
class AbstractDeviceWidget;
class NetworkMainWidget : public ScrollFrame
{
    Q_OBJECT
public:
    explicit NetworkMainWidget(QWidget *parent = 0);

    DBusNetwork *dbusNetwork() const;

private:
    void updateUI();
    void initUI();

    int m_wiredCount = 0;
    int m_modemCount = 0;
    int m_wirelessCount = 0;
    DBusNetwork *m_dbusNetwork;
    QMap<QString, AbstractDeviceWidget*> m_mapPathToObject;
    VPNConnectsWidget *m_vpnConnectsWidget;
};

#endif // NETWORKMAINWIDGET_H
