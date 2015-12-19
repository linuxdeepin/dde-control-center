#ifndef WIRELESSNETWORKLISTITEM_H
#define WIRELESSNETWORKLISTITEM_H

#include <QPointer>

#include "abstractdevicewidget.h"

class InputPasswordDialog;
class WirelessNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit WirelessNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);
    ~WirelessNetworkListItem();

private slots:
    void onItemClicked();
    void onConnectsChanged();
    void updateItemIndex(int strength);
    void updateActiveApState();
    void onActiveConnectionsChanged();
    NetworkGenericListItem *addAccessPoint(const QVariantMap &map);

private:
    void init();

    void closeInputDialog();

    QPointer<NetworkGenericListItem> m_activeItem;
    QPointer<InputPasswordDialog> m_ddialog;
    QMap<QString, NetworkGenericListItem*> m_mapApPathToItem;
    QMap<QString, NetworkGenericListItem*> m_mapApSsidToItem;
    QMap<QString, NetworkGenericListItem*> m_mapApUuidToItem;

    QString m_targetConnectPath;
    QString m_tragetConnectUuid;
};

#endif // WIRELESSNETWORKLISTITEM_H
