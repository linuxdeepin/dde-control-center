#ifndef VPNCONNECTSWIDGET_H
#define VPNCONNECTSWIDGET_H

#include "abstractdevicewidget.h"

class VPNConnectsWidget : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void onConnectsChanged();

private:
    QMap<QString, NetworkGenericListItem*> m_mapVpnPathToItem;
};

#endif // VPNCONNECTSWIDGET_H
