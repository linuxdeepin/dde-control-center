#ifndef VPNCONNECTSWIDGET_H
#define VPNCONNECTSWIDGET_H

#include "abstractdevicewidget.h"

class VPNConnectsWidget : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent = 0);

    QString path() const;

private slots:
    void onConnectsChanged();
    void onItemClicked();
    void onClearButtonClicked();
    void onItemStateChanged(int state);
    void onActiveConnectionsChanged();

private:
    QMap<QString, NetworkGenericListItem*> m_mapVpnPathToItem;
};

#endif // VPNCONNECTSWIDGET_H
