#ifndef WIRELESSNETWORKLISTITEM_H
#define WIRELESSNETWORKLISTITEM_H

#include "abstractdevicewidget.h"

class WirelessNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit WirelessNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void onItemClicked();
    void onDbusConnectsChanged();
    void updateItemIndex(int strength);
    NetworkGenericListItem *addAccessPoint(const QVariantMap &map);

private:
    void init();

    bool m_inputPasswording = false;
    NetworkGenericListItem *m_activeItem = nullptr;
    QMap<QString, NetworkGenericListItem*> m_mapApPathToItem;
    QMap<QString, NetworkGenericListItem*> m_mapApSsidToItem;
};

#endif // WIRELESSNETWORKLISTITEM_H
