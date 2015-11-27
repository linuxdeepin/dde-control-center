#ifndef WIREDNETWORKLISTITEM_H
#define WIREDNETWORKLISTITEM_H

#include "abstractdevicewidget.h"

class WiredNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit WiredNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void init();
    void onConnectsChanged();
    void onItemClicked();
    void onClearButtonClicked();
    void onItemStateChanged(int state);

private:
    QMap<QString, NetworkGenericListItem*> m_mapPppoePathToItem;
};

#endif // WIREDNETWORKLISTITEM_H
