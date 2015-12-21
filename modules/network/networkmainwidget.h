#ifndef NETWORKMAINWIDGET_H
#define NETWORKMAINWIDGET_H

#include "scrollframe.h"
#include "dbus/dbusnetwork.h"
#include "networkglobal.h"

class QVBoxLayout;
class WiredNetworkListItem;
class WirelessNetworkListItem;
class VPNConnectsWidget;
class AbstractDeviceWidget;
class Network;
class NetworkMainWidget : public ScrollFrame
{
    Q_OBJECT

public:
    explicit NetworkMainWidget(Network *network, QWidget *parent = 0);

    DBusNetwork *dbusNetwork() const;

    bool dccIsVisible() const;

private:
    void updateDeviceByMap(const QString &type, const QVariantMap &map,
                           int index, QList<AbstractDeviceWidget*> &tmp_list);
    void updateUI();
    void initUI();

    int m_wiredCount = 0;
    int m_modemCount = 0;
    int m_wirelessCount = 0;
    DBusNetwork *m_dbusNetwork;
    QMap<QString, AbstractDeviceWidget*> m_mapPathToObject;
    VPNConnectsWidget *m_vpnConnectsWidget;

    Network *m_networkModule;

signals:
    void dccVisibleChanged(bool visible);
};

#endif // NETWORKMAINWIDGET_H
